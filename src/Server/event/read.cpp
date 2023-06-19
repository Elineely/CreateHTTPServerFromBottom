#include "HttpProcessor.hpp"
#include "Log.hpp"
#include "ResponseGenerator.hpp"
#include "Server.hpp"
#include "ServerFinder.hpp"

#define CHILD_PROCESS 0

/*
  [SUMMARY]
  - 서버 소켓에 발생하는 이벤트를 처리하는 함수입니다.
  - 발생하는 이벤트는 총 2가지 입니다.

  1. 서버 소켓에 오류가 발생하는 경우 (current_event->flag & EV_ERROR)
  2. 클라이언트와 TCP 연결을 맺는 경우
*/
void Server::serverReadEvent(struct kevent *current_event)
{
  t_event_udata *current_udata;

  current_udata = static_cast<t_event_udata *>(current_event->udata);
  if (current_event->flags & EV_ERROR)
  {
    LOG_INFO("💥 Server socket(fd: %d) error. it will be closed. 💥", current_event->ident);
    disconnectSocket(current_event->ident);
    ft_delete(&current_udata);
    return;
  }

  int client_sock;
  Request *request;
  Response *response;
  t_event_udata *udata;

  client_sock = clientReadAccept(current_event);
  fcntl(client_sock, F_SETFL, O_NONBLOCK);

  request = new Request();
  printf("serverReadEvent request: %p\n", request);  // fish

  response = new Response();
  printf("serverReadEvent response: %p\n", response);  // suspicious

  udata =
      new t_event_udata(CLIENT, current_udata->m_servers, request, response);
  printf("serverReadEvent udata: %p\n", udata);

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
    ft_delete(&current_udata->m_request);
    ft_delete(&current_udata->m_response);
    ft_delete(&current_udata);
    return;
  }
  current_udata->m_parser.readBuffer(buff, recv_size,
                                     *current_udata->m_request);
}

void Server::clientReadEvent(struct kevent *current_event)
{
  if (current_event->flags & EV_EOF)
  {
    LOG_INFO("💥 Client socket(fd: %d) will be close 💥", current_event->ident);
    t_event_udata *current_udata =
        static_cast<t_event_udata *>(current_event->udata);

    LOG_DEBUG("method: %s, status code: %d",
              current_udata->m_request->method.c_str(),
              current_udata->m_response->status_code);

    ft_delete(&current_udata->m_request);
    ft_delete(&current_udata->m_response);
    ft_delete(&current_udata);
    disconnectSocket(current_event->ident);
    return;
  }

  t_event_udata *current_udata;

  current_udata = static_cast<t_event_udata *>(current_event->udata);
  readClientSocketBuffer(current_event, current_udata);
  if (current_udata->m_request->validation_phase != COMPLETE)
  {
    return;
  }

  struct Request &request = *current_udata->m_request;
  struct Response &response = *current_udata->m_response;
  ServerFinder server_finder(request, current_udata->m_servers);
  HttpProcessor http_processor(request, response, server_finder.get_server());

  // cgi 분기 확인
  if (response.cgi_flag == true)
  {
    addCgiRequestEvent(current_event, current_udata, request, response);
  }
  else
  {
    addStaticRequestEvent(current_event, current_udata, request, response);
  }

  ft_delete(&current_udata->m_request);
  current_udata->m_request = new Request();
  printf("addCgiRequestEvent current_udata->m_request %p\n",
         current_udata->m_request);  // TODO

  ft_delete(&current_udata->m_response);
  current_udata->m_response = new Response();
  printf("addCgiRequestEvent current_udata->m_response %p\n",
         current_udata->m_response);  // TODO

  Parser new_parser;
  current_udata->m_parser = new_parser;
}

void Server::addCgiRequestEvent(struct kevent *current_event,
                                t_event_udata *current_udata,
                                struct Request &request,
                                struct Response &response)
{
  // Set up the event structure
  Request *new_request = new Request(*current_udata->m_request);
  printf("[addCgiRequestEvent] new_request: %p\n", new_request);  // TODO

  t_event_udata *udata =
      new t_event_udata(PIPE, current_udata->m_servers, new_request, NULL);
  printf("[addCgiRequestEvent] udata: %p\n", udata);

  new_request = new Request(*current_udata->m_request);
  printf("[addCgiRequestEvent] new_request: %p\n", new_request);  // TODO

  t_event_udata *udata2 =
      new t_event_udata(PROCESS, current_udata->m_servers, new_request, NULL);
  printf("[addCgiRequestEvent] udata2: %p\n", udata2);  // TODO

  udata->m_read_pipe_fd = response.read_pipe_fd;
  udata->m_write_pipe_fd = response.write_pipe_fd;
  udata->m_child_pid = response.cgi_child_pid;
  udata->m_client_sock = current_event->ident;
  udata->m_response = new Response(response);
  printf("[addCgiRequestEvent] udata->m_response %p\n",
         udata->m_response);  // TODO
  udata->m_other_udata = udata2;

  udata2->m_read_pipe_fd = response.read_pipe_fd;
  udata2->m_write_pipe_fd = response.write_pipe_fd;
  udata2->m_child_pid = response.cgi_child_pid;
  udata2->m_client_sock = current_event->ident;
  udata2->m_response = new Response(response);
  printf("[addCgiRequestEvent] udata2->m_response %p\n",
         udata2->m_response);  // TODO
  udata2->m_other_udata = udata;

  if (request.method == "POST")
  {
    Request *new_request_2 = new Request(*current_udata->m_request);
    printf("[addCgiRequestEvent] new_request_2: %p", new_request_2);

    t_event_udata *udata1 =
        new t_event_udata(PIPE, current_udata->m_servers, new_request_2, NULL);
    printf("[addCgiRequestEvent] udata1: %p\n", udata1);

    udata1->m_read_pipe_fd = response.read_pipe_fd;
    udata1->m_write_pipe_fd = response.write_pipe_fd;
    udata1->m_child_pid = response.cgi_child_pid;
    udata1->m_client_sock = current_event->ident;
    udata1->m_response = new Response();
    printf("[addCgiRequestEvent] udata1->m_response %p\n",
           udata1->m_response);  // TODO
    fcntl(response.write_pipe_fd, F_SETFL, O_NONBLOCK);
    addEventToChangeList(m_kqueue.change_list, response.write_pipe_fd,
                         EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, udata1);
  }

  fcntl(response.read_pipe_fd, F_SETFL, O_NONBLOCK);
  addEventToChangeList(m_kqueue.change_list, response.read_pipe_fd, EVFILT_READ,
                       EV_ADD | EV_ENABLE, 0, 0, udata);
  addEventToChangeList(m_kqueue.change_list, response.cgi_child_pid,
                       EVFILT_TIMER, EV_ADD | EV_ONESHOT, NOTE_SECONDS,
                       DEFAULT_TIMEOUT_SECOND, udata2);
}

void Server::addStaticRequestEvent(struct kevent *current_event,
                                   t_event_udata *current_udata,
                                   struct Request &request,
                                   struct Response &response)
{
  Request *new_request;
  Response *new_response;

  if (response.static_read_file_fd != -1)
  {
    off_t file_size;
    t_event_udata *udata;

    file_size = lseek(response.static_read_file_fd, 0, SEEK_END);
    if (file_size == -1)
    {
      LOG_ERROR("failed lseek");
      throw INTERNAL_SERVER_ERROR_500;
    }
    lseek(response.static_read_file_fd, 0, SEEK_SET);
    response.static_read_file_size = file_size;
    response.body.reserve(file_size);

    new_request = new Request(request);
    new_response = new Response(response);
    printf("[addStaticRequestEvent] new_request: %p\n", new_request);    // TODO
    printf("[addStaticRequestEvent] new_response: %p\n", new_response);  // TODO
    udata = new t_event_udata(STATIC_FILE, current_udata->m_servers,
                              new_request, new_response);
    udata->m_client_sock = current_event->ident;
    addEventToChangeList(m_kqueue.change_list, response.static_read_file_fd,
                         EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, udata);
    return;
  }
  else if (response.static_write_file_fd != -1)
  {
    t_event_udata *udata;

    new_request = new Request(request);
    new_response = new Response(response);
    printf("[addStaticRequestEvent] new_request: %p\n", new_request);    // TODO
    printf("[addStaticRequestEvent] new_response: %p\n", new_response);  // TODO
    udata = new t_event_udata(STATIC_FILE, current_udata->m_servers,
                              new_request, new_response);
    udata->m_client_sock = current_event->ident;
    addEventToChangeList(m_kqueue.change_list, response.static_write_file_fd,
                         EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, udata);
    return;
    // 파일을 만들기
    // 파일에 쓰기
    // 파일이 문제가 있다면 에러 throw
    // content_stream.write(&m_request_data.body[0],
    // m_request_data.body.size());
  }

  ResponseGenerator response_generator(request, response);
  std::vector<char> response_message;
  t_event_udata *udata;

  response_message = response_generator.generateResponseMessage();
  udata = new t_event_udata(CLIENT, current_udata->m_servers, NULL, NULL);
  Log::printRequestResult(current_udata);
  printf("[addStaticRequestEvent] udata: %p\n", udata);  // TODO
  udata->m_response_write.message = response_message;
  udata->m_response_write.offset = 0;
  udata->m_response_write.length = response_message.size();

  addEventToChangeList(m_kqueue.change_list, current_event->ident, EVFILT_WRITE,
                       EV_ADD | EV_ENABLE, 0, 0, udata);
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
    printf("[pipeReadEvent] buf %p\n", buf);  // TODO
    std::memmove(buf, temp_buf, read_byte);
    current_udata->m_read_buffer.push_back(buf);
    current_udata->m_read_bytes.push_back(read_byte);
    current_udata->m_total_read_byte += read_byte;
    return;
  }
  wait(NULL);
  if (current_event->flags & EV_EOF)
  {
    current_udata->m_response->body.reserve(current_udata->m_total_read_byte);
    for (int i = 0; i < current_udata->m_read_buffer.size(); ++i)
    {
      for (int j = 0; j < current_udata->m_read_bytes[i]; ++j)
      {
        current_udata->m_response->body.push_back(
            current_udata->m_read_buffer[i][j]);
      }
      delete current_udata->m_read_buffer[i];
    }

    close(current_event->ident);
    ResponseGenerator ok(*current_udata->m_request, *current_udata->m_response);
    t_event_udata *udata;

    udata =
        new t_event_udata(CLIENT, current_udata->m_servers,
                          current_udata->m_request, current_udata->m_response);
    printf("[pipeReadEvent] udata: %p\n", udata);  // TODO

    udata->m_response_write.message = ok.generateResponseMessage();
    udata->m_response_write.offset = 0;
    udata->m_response_write.length = udata->m_response_write.message.size();

    Log::printRequestResult(current_udata);
    addEventToChangeList(m_kqueue.change_list, current_udata->m_client_sock,
                         EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, udata);
    addEventToChangeList(m_kqueue.change_list, current_udata->m_child_pid,
                         EVFILT_TIMER, EV_DELETE, 0, 0, NULL);
    ft_delete(&(current_udata->m_other_udata->m_request));
    ft_delete(&(current_udata->m_other_udata->m_response));
    ft_delete(&(current_udata->m_other_udata));
    ft_delete(&current_udata);
  }
}

void Server::staticFileReadEvent(struct kevent *current_event)
{
  ssize_t read_byte;
  char buf[BUF_SIZE];
  t_event_udata *current_udata;

  current_udata = static_cast<t_event_udata *>(current_event->udata);
  read_byte = read(current_event->ident, buf, BUF_SIZE);
  if (read_byte > 0)
  {
    for (ssize_t idx = 0; idx < read_byte; ++idx)
    {
      current_udata->m_response->body.push_back(buf[idx]);
    }
  }
  if (read_byte < BUF_SIZE)
  {
    LOG_INFO("static read file eof reached");
    close(current_event->ident);
    t_event_udata *udata;
    Request *request = current_udata->m_request;
    Response *response = current_udata->m_response;
    ResponseGenerator response_generator(*request, *response);

    udata = new t_event_udata(CLIENT, current_udata->m_servers, NULL, NULL);
    printf("[staticFileReadEvent] udata: %p\n", udata);  // TODO
    udata->m_response_write.message =
        response_generator.generateResponseMessage();
    udata->m_response_write.offset = 0;
    udata->m_response_write.length = udata->m_response_write.message.size();

    Log::printRequestResult(current_udata);
    addEventToChangeList(m_kqueue.change_list, current_udata->m_client_sock,
                         EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, udata);
    ft_delete(&current_udata->m_request);
    ft_delete(&current_udata->m_response);
    ft_delete(&current_udata);
  }
}