#include "HttpProcessor.hpp"
#include "Log.hpp"
#include "ResponseGenerator.hpp"
#include "Server.hpp"

#define CHILD_PROCESS 0

void Server::serverReadEvent(struct kevent *current_event)
{
  int client_sock;
  t_event_udata *current_udata;
  t_event_udata *udata;

  current_udata = static_cast<t_event_udata *>(current_event->udata);
  if (current_event->flags & EV_EOF)
  {
    disconnectSocket(current_event->ident);
    if (current_udata->m_other_udata != NULL)
      delete current_udata->m_other_udata;
    delete current_event->udata;
    return;
  }
  client_sock = clientReadAccept(current_event);
  fcntl(client_sock, F_SETFL, O_NONBLOCK);

  udata = new t_event_udata(CLIENT, current_udata->m_server);

  addEventToChangeList(m_kqueue.change_list, client_sock, EVFILT_READ,
                       EV_ADD | EV_ENABLE, 0, 0, udata);
}

void Server::readClientSocketBuffer(struct kevent *current_event,
                                    t_event_udata *current_udata)
{
  int recv_size;
  char buff[BUF_SIZE];

  recv_size = recv(current_event->ident, buff, sizeof(buff), 0);
  if (recv_size == 0)
  {
    disconnectSocket(current_event->ident);
    return ;
  }
  current_udata->m_parser.readBuffer(buff, recv_size);
}

void Server::addCgiRequestEvent(struct kevent *current_event,
                                t_event_udata *current_udata,
                                struct Request &request,
                                struct Response &response)
{
  // Set up the event structure
  t_event_udata *udata = new t_event_udata(PIPE);
  t_event_udata *udata2 = new t_event_udata(PROCESS);

  udata->m_read_pipe_fd = response.read_pipe_fd;
  udata->m_write_pipe_fd = response.write_pipe_fd;
  udata->m_child_pid = response.cgi_child_pid;
  udata->m_client_sock = current_event->ident;
  udata->m_parser = current_udata->m_parser;
  udata->m_response = response;
  udata->m_other_udata = udata2;

  udata2->m_read_pipe_fd = response.read_pipe_fd;
  udata2->m_write_pipe_fd = response.write_pipe_fd;
  udata2->m_child_pid = response.cgi_child_pid;
  udata2->m_client_sock = current_event->ident;
  udata2->m_parser = current_udata->m_parser;
  udata2->m_response = response;
  udata2->m_other_udata = udata;

  if (request.method == "POST")
  {
    t_event_udata *udata1 = new t_event_udata(PIPE);
    udata1->m_read_pipe_fd = response.read_pipe_fd;
    udata1->m_write_pipe_fd = response.write_pipe_fd;
    udata1->m_child_pid = response.cgi_child_pid;
    udata1->m_client_sock = current_event->ident;
    udata1->m_parser = current_udata->m_parser;
    udata1->m_response = response;
    fcntl(response.write_pipe_fd, F_SETFL, O_NONBLOCK);
    addEventToChangeList(m_kqueue.change_list, response.write_pipe_fd,
                         EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, udata1);
  }

  fcntl(response.read_pipe_fd, F_SETFL, O_NONBLOCK);
  addEventToChangeList(m_kqueue.change_list, response.read_pipe_fd, EVFILT_READ,
                       EV_ADD | EV_ENABLE, 0, 0, udata);
  addEventToChangeList(m_kqueue.change_list, response.cgi_child_pid,
                       EVFILT_TIMER, EV_ADD | EV_ONESHOT, NOTE_SECONDS, 600,
                       udata2);
}

void Server::addStaticRequestEvent(struct kevent *current_event,
                                   t_event_udata *current_udata,
                                   struct Request &request,
                                   struct Response &response)
{
  std::vector<char> response_message;
  ResponseGenerator response_generator(request, response);
  t_event_udata *udata;

  response_message = response_generator.generateResponseMessage();
  udata = new t_event_udata(CLIENT);
  udata->m_response_write.message = response_message;
  udata->m_response_write.length = response_message.size();

  addEventToChangeList(m_kqueue.change_list, current_event->ident, EVFILT_WRITE,
                       EV_ADD | EV_ENABLE, 0, 0, udata);
}

void Server::clientReadEvent(struct kevent *current_event)
{
  if (current_event->flags & EV_EOF)
  {
    LOG_INFO("💥 Client socket(fd: %d) will be close 💥", current_event->ident);
    t_event_udata *current_udata =
        static_cast<t_event_udata *>(current_event->udata);
    if (current_udata->m_other_udata != NULL)
    {
      delete current_udata->m_other_udata;
    }
    delete current_udata;
    disconnectSocket(current_event->ident);
    return;
  }

  t_event_udata *current_udata;

  current_udata = static_cast<t_event_udata *>(current_event->udata);
  readClientSocketBuffer(current_event, current_udata);
  if (current_udata->m_parser.get_validation_phase() != COMPLETE)
  {
    return;
  }

  struct Request &request = current_udata->m_parser.get_request();
  HttpProcessor http_processor(request, current_udata->m_server);
  struct Response &response = http_processor.get_m_response();

  // cgi 분기 확인
  if (response.cgi_flag == true)
  {
    addCgiRequestEvent(current_event, current_udata, request, response);
  }
  else
  {
    addStaticRequestEvent(current_event, current_udata, request, response);
  }

  Parser new_parser;
  current_udata->m_parser = new_parser;
}

void Server::pipeReadEvent(struct kevent *current_event)
{
  char *buf;
  char temp_buf[BUF_SIZE];
  ssize_t read_byte;
  t_event_udata *current_udata;

  current_udata = static_cast<t_event_udata *>(current_event->udata);
  read_byte = read(current_event->ident, temp_buf, BUF_SIZE);

  if (read_byte > 0)
  {
    buf = new char[read_byte]();
    std::memmove(buf, temp_buf, read_byte);
    current_udata->m_read_buffer.push_back(buf);
    current_udata->m_read_bytes.push_back(read_byte);
    current_udata->m_total_read_byte += read_byte;
    return;
  }
  wait(NULL);
  if (current_event->flags & EV_EOF)
  {
    current_udata->m_response.body.reserve(current_udata->m_total_read_byte);
    for (int i = 0; i < current_udata->m_read_buffer.size(); ++i)
    {
      for (int j = 0; j < current_udata->m_read_bytes[i]; ++j)
      {
        current_udata->m_response.body.push_back(
            current_udata->m_read_buffer[i][j]);
      }
      delete current_udata->m_read_buffer[i];
    }

    close(current_event->ident);
    ResponseGenerator ok(current_udata->m_parser.get_request(),
                         current_udata->m_response);
    std::vector<char> &response_message = ok.generateResponseMessage();
    t_event_udata *udata;

    udata =
        new t_event_udata(CLIENT, response_message, response_message.size());
    addEventToChangeList(m_kqueue.change_list, current_udata->m_client_sock,
                         EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, udata);
    addEventToChangeList(m_kqueue.change_list, current_udata->m_child_pid,
                         EVFILT_TIMER, EV_DELETE, 0, 0, NULL);
    delete current_udata->m_other_udata;
    delete current_udata;
  }
}