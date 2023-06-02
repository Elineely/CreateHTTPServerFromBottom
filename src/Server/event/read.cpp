#include "Server.hpp"
#include "HttpProcessor.hpp"
#include "ResponseGenerator.hpp"

#define CHILD_PROCESS 0

void Server::serverReadEvent(struct kevent *current_event)
{
  std::cout << "🖥  SERVER READ EVENT  🖥" << std::endl;
  int client_sock;
  int client_addr_size;
  struct sockaddr_in client_addr;

  client_sock = accept(current_event->ident, (struct sockaddr *)&client_addr,
                       reinterpret_cast<socklen_t *>(&client_addr_size));
  if (client_sock == -1)
  {
    ft_error(1, strerror(errno));
    return;
  }
  std::cout << "🏃 Client ID: " << client_sock << " is connected. 🏃"
            << std::endl;

  fcntl(client_sock, F_SETFL, O_NONBLOCK);

  t_event_udata *udata = new t_event_udata(CLIENT);
  t_event_udata *current_udata = static_cast<t_event_udata*>(current_event->udata);

  udata->m_server = current_udata->m_server;
  
  AddEventToChangeList(m_kqueue.change_list, client_sock, EVFILT_READ,
                       EV_ADD | EV_ENABLE, 0, 0, udata);
}

void Server::clientReadEvent(struct kevent *current_event)
{
  std::cout << "📖 CLIENT_READ EVENT 📖" << std::endl;
  if (current_event->flags & EV_EOF)
  {
    std::cerr << "closing" << std::endl;
    disconnect_socket(current_event->ident);
    return;
  }

  t_event_udata *udata = static_cast<t_event_udata *>(current_event->udata);

  char buff[BUF_SIZE];
  std::memset(buff, 0, BUF_SIZE);
  int recv_size = recv(current_event->ident, buff, sizeof(buff), 0);
  udata->m_parser.readBuffer(buff);
  if (udata->m_parser.get_validation_phase() != COMPLETE)
  {
    return;
  }
  
  struct Request& request = udata->m_parser.get_request();
  struct Response response;

  // http_processor 호출
  HttpProcessor http_processor(request, udata->m_server);

  // cgi 분기 확인
  response = http_processor.get_m_response();
  if (response.cgi_flag == true)
  {
    // Set up the event structure
    t_event_udata *udata = new t_event_udata(PIPE);
    t_event_udata *udata2 = new t_event_udata(PROCESS);

    udata->m_pipe_read_fd = response.read_pipe_fd;
    udata->m_child_pid = response.cgi_child_pid;
    udata->m_client_sock = current_event->ident;
    udata2->m_pipe_read_fd = response.read_pipe_fd;
    udata2->m_child_pid = response.cgi_child_pid;
    udata2->m_client_sock = current_event->ident;

    fcntl(response.read_pipe_fd, F_SETFL, O_NONBLOCK);
    AddEventToChangeList(m_kqueue.change_list, response.read_pipe_fd, EVFILT_READ,
                        EV_ADD | EV_ENABLE, 0, 0, udata);
    AddEventToChangeList(m_kqueue.change_list, response.cgi_child_pid, EVFILT_TIMER,
                        EV_ADD | EV_ONESHOT, NOTE_SECONDS, 2, udata2);
  }
  else
  {
    std::vector<char> response_message;
    std::cout << "http code " << http_processor.get_m_response().status_code  << std::endl;
    if (http_processor.get_m_response().status_code == OK_200 \
        || http_processor.get_m_response().status_code == FOUND_302)
    {
      ResponseGenerator ok(request, http_processor.get_m_response());
      //vector<char> 진짜 response message
      response_message = ok.generateResponseMessage();
    }
    else
    {
      ResponseGenerator not_ok(request, http_processor.get_m_response());
     
      //vector<char> 진짜 response message
      response_message = not_ok.generateErrorResponseMessage();
    }
    t_event_udata *udata = new t_event_udata(CLIENT);
    udata->m_response.message = &response_message[0];
    udata->m_response.length = response_message.size();

    AddEventToChangeList(m_kqueue.change_list, current_event->ident,
                        EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, udata);
  }
}

void Server::pipeReadEvent(struct kevent *current_event)
{
  std::cout << "💧 PIPE READ EVENT 💧" << std::endl;

  char buf[BUF_SIZE];
  std::memset(buf, 0, BUF_SIZE);
  t_event_udata *current_udata =
      static_cast<t_event_udata *>(current_event->udata);

  ssize_t read_byte = read(current_udata->m_pipe_read_fd, buf, BUF_SIZE);
  if (read_byte > 0)
  {
    current_udata->m_result.append(buf);
    return;
  }
  wait(NULL);
  if (current_event->flags & EV_EOF)
  {
    std::cout << "💩 PIPE EOF EVENT 💩" << std::endl;
    close(current_event->ident);

    // char *message = getHttpMessage();  // 인자로 response 전달
    const char *message = current_udata->m_result.c_str();

    t_event_udata *udata =
        new t_event_udata(CLIENT, message, ft_strlen(message));

    AddEventToChangeList(m_kqueue.change_list, current_udata->m_client_sock,
                         EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, udata);
    AddEventToChangeList(m_kqueue.change_list, current_udata->m_child_pid,
                         EVFILT_TIMER, EV_DELETE, 0, 0, NULL);
  }
}
