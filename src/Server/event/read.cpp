#include "HttpProcessor.hpp"
#include "Log.hpp"
#include "ResponseGenerator.hpp"
#include "Server.hpp"

#define CHILD_PROCESS 0

void Server::serverReadEvent(struct kevent *current_event)
{
  int client_sock;
  int client_addr_size;
  struct sockaddr_in client_addr;

  LOG_INFO("🖥  SERVER READ EVENT  🖥");
  client_sock = accept(current_event->ident, (struct sockaddr *)&client_addr,
                       reinterpret_cast<socklen_t *>(&client_addr_size));
  if (client_sock == -1)
  {
    LOG_ERROR("Failed to accept client socket (strerror: %s)", strerror(errno));
    return;
  }
  LOG_INFO("🌵 Client Socket fd %d is created 🌵", client_sock);

  fcntl(client_sock, F_SETFL, O_NONBLOCK);

  t_event_udata *current_udata =
      static_cast<t_event_udata *>(current_event->udata);
  t_event_udata *udata = new t_event_udata(CLIENT, current_udata->m_server);

  // udata->m_server = current_udata->m_server;

  AddEventToChangeList(m_kqueue.change_list, client_sock, EVFILT_READ,
                       EV_ADD | EV_ENABLE, 0, 0, udata);
}

void Server::clientReadEvent(struct kevent *current_event)
{
  LOG_INFO("📖 CLIENT_READ EVENT 📖");

  if (current_event->flags & EV_EOF)
  {
    LOG_INFO("💥 Client socket(fd: %d) will be close 💥", current_event->ident);
    disconnect_socket(current_event->ident);
    return;
  }

  t_event_udata *current_udata = static_cast<t_event_udata *>(current_event->udata);

  char buff[BUF_SIZE];
  std::memset(buff, 0, BUF_SIZE);
  int recv_size = recv(current_event->ident, buff, sizeof(buff), 0);
  current_udata->m_parser.readBuffer(buff, recv_size);
  if (current_udata->m_parser.get_validation_phase() != COMPLETE)
  {
    return;
  }


  struct Request &request = current_udata->m_parser.get_request(); 
  struct Response response;

  // http_processor 호출
  HttpProcessor http_processor(request, current_udata->m_server);

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
    udata->m_parser = current_udata->m_parser;
    udata->m_response = response;
    udata->m_other_udata = udata2;

    udata2->m_pipe_read_fd = response.read_pipe_fd;
    udata2->m_child_pid = response.cgi_child_pid;
    udata2->m_client_sock = current_event->ident;
    udata2->m_parser = current_udata->m_parser;
    udata2->m_response = response;
    udata2->m_other_udata = udata;

    fcntl(response.read_pipe_fd, F_SETFL, O_NONBLOCK);
    AddEventToChangeList(m_kqueue.change_list, response.read_pipe_fd,
                         EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, udata);
    AddEventToChangeList(m_kqueue.change_list, response.cgi_child_pid,
                         EVFILT_TIMER, EV_ADD | EV_ONESHOT, NOTE_SECONDS, 10,
                         udata2);
    Parser new_parser(current_udata->m_server.max_body_size[0]);
    current_udata->m_parser = new_parser;
  }
  else
  {
    std::vector<char> response_message;
    LOG_DEBUG("http response code: %d",
              http_processor.get_m_response().status_code);

      ResponseGenerator response_generator(request, http_processor.get_m_response());
      // vector<char> 진짜 response message
      response_message = response_generator.generateResponseMessage();
    t_event_udata *udata = new t_event_udata(CLIENT);
    t_event_udata *current_udata = (t_event_udata *)current_event->udata;
    // t_server crrent_m_server = current_udata->m_server;
    udata->m_response_write.message = response_message;
    udata->m_response_write.length = response_message.size();

    AddEventToChangeList(m_kqueue.change_list, current_event->ident,
                         EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, udata);
    Parser new_parser(current_udata->m_server.max_body_size[0]);
    current_udata->m_parser = new_parser;
  }
}

void Server::pipeReadEvent(struct kevent *current_event)
{
  LOG_INFO("💧 PIPE READ EVENT 💧");

  char buf[BUF_SIZE];
  std::memset(buf, 0, BUF_SIZE);
  t_event_udata *current_udata =
      static_cast<t_event_udata *>(current_event->udata);

  ssize_t read_byte = read(current_udata->m_pipe_read_fd, buf, BUF_SIZE);
  if (read_byte > 0)
  {
    for (int i=0; i<read_byte; ++i)
    {
      current_udata->m_result.push_back(buf[i]);
    }
    return;
  }
  wait(NULL);
  if (current_event->flags & EV_EOF)
  {
    LOG_INFO("💩 PIPE EOF EVENT 💩");

    close(current_event->ident);
    std::vector<char> response_message;
    
    current_udata->m_response.body = current_udata->m_result;
    ResponseGenerator ok(current_udata->m_parser.get_request(), current_udata->m_response);
    response_message = ok.generateResponseMessage();
    t_event_udata *udata =
        new t_event_udata(CLIENT, response_message, response_message.size());

    AddEventToChangeList(m_kqueue.change_list, current_udata->m_client_sock,
                         EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, udata);
    AddEventToChangeList(m_kqueue.change_list, current_udata->m_child_pid,
                         EVFILT_TIMER, EV_DELETE, 0, 0, NULL);
    delete current_udata->m_other_udata;
    delete current_udata;
  }
}