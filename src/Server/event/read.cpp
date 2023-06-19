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
    LOG_INFO("💥 Server socket(fd: %d) error. 💥", current_event->ident);
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
  printf("serverReadEvent request: %p\n", request);  // TODO

  response = new Response();
  printf("serverReadEvent response: %p\n", response);  // TODO

  udata =
      new t_event_udata(CLIENT, current_udata->m_servers, request, response);
  printf("serverReadEvent udata: %p\n", udata);

  addEventToChangeList(m_kqueue.change_list, client_sock, EVFILT_READ,
                       EV_ADD | EV_ENABLE, 0, 0, udata);
}

/*
  [SUMMARY]
  - 클라이언트가 보낸 데이터를 클라이언트 소켓의 read buffer 에서 읽어옵니다.
  - recv 반환 값 0은 클라이언트가 연결을 끊기 위해 EOF 를 보낸 것을 의미합니다.
*/
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

/*
  [SUMMARY]
  - 클라이언트 소켓에 발생한 EVFILT_READ 이벤트를 감지합니다.
  - 발생하는 이벤트는 총 2가지 입니다.

  1. 클라이언트 소켓의 연결을 끊는 경우 (current_event->flags & EV_EOF)
  2. 클라이언트 소켓의 read buffer 에 담긴 데이터를 읽어오는 경우
    - CGI 요청과 정적 요청을 구분해서 처리합니다.
    - Parser 는 동일한 클라이언트의 다음 요청을 처리하기 위해 초기화 합니다.
*/
void Server::clientReadEvent(struct kevent *current_event)
{
  t_event_udata *current_udata;

  current_udata = static_cast<t_event_udata *>(current_event->udata);
  if (current_event->flags & EV_EOF)
  {
    LOG_INFO("💥 Client socket(fd: %d) will be close 💥", current_event->ident);
    LOG_DEBUG("method: %s, status code: %d",
              current_udata->m_request->method.c_str(),
              current_udata->m_response->status_code);

    ft_delete(&current_udata->m_request);
    ft_delete(&current_udata->m_response);
    ft_delete(&current_udata);
    disconnectSocket(current_event->ident);
    return;
  }

  readClientSocketBuffer(current_event, current_udata);
  if (current_udata->m_request->validation_phase != COMPLETE)
  {
    return;
  }

  struct Request &request = *current_udata->m_request;
  struct Response &response = *current_udata->m_response;
  ServerFinder server_finder(request, current_udata->m_servers);
  HttpProcessor http_processor(request, response, server_finder.get_server());
  Parser new_parser;

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
  ft_delete(&current_udata->m_response);

  current_udata->m_request = new Request();
  printf("addCgiRequestEvent current_udata->m_request %p\n",
         current_udata->m_request);  // TODO
  current_udata->m_response = new Response();
  printf("addCgiRequestEvent current_udata->m_response %p\n",
         current_udata->m_response);  // TODO

  current_udata->m_parser = new_parser;
}

/*
  [SUMMARY]
  - CGI 요청을 처리하기 위한 함수입니다.
  - GET, POST 요청 시 공통으로 등록하는 이벤트는 총 2가지 입니다.
    1. pipe 의 read buffer 에 발생하는 EVFILT_READ 이벤트
    2. CGI 프로그램이 일정 시간 동안 응답이 없어서 TIMEOUT 이 발생하는 이벤트
  - POST 요청인 경우에는 이벤트를 하나 더 추가합니다.
    3. CGI 프로그램의 표준 입력으로 데이터를 넘겨주기 위해
        pipe 의 write buffer 에 데이터를 입력하는 이벤트
  - pipe read/write buffer 에 대해 등록하는 이유는
    pipe 의 최대 버퍼 크기 제한이 있어서
    한 번에 데이터를 읽고 쓰는 것이 불가능 하기 때문입니다.
*/
t_event_udata *Server::createUdata(e_event_type type,
                                   struct kevent *current_event,
                                   t_event_udata *current_udata,
                                   struct Response &response)
{
  Request *new_request;
  Response *new_response;
  t_event_udata *udata;

  new_request = new Request(*current_udata->m_request);
  printf("[createUdata] new_request: %p\n", new_request);  // TODO

  new_response = new Response(response);
  printf("[createUdata] new_response: %p\n", new_response);  // TODO

  udata = new t_event_udata(type, current_udata->m_servers, new_request,
                            new_response);
  udata->m_read_pipe_fd = response.read_pipe_fd;
  udata->m_write_pipe_fd = response.write_pipe_fd;
  udata->m_child_pid = response.cgi_child_pid;
  udata->m_client_sock = current_event->ident;

  return (udata);
}

/*
  [SUMMARY]
  - CGI 요청을 처리하기 위한 함수입니다.
  - GET, POST 요청 시 공통으로 등록하는 이벤트는 총 2가지 입니다.
    1. pipe 의 read buffer 에 발생하는 EVFILT_READ 이벤트
    2. CGI 프로그램이 일정 시간 동안 응답이 없어서 TIMEOUT 이 발생하는 이벤트
  - POST 요청인 경우에는 이벤트를 하나 더 추가합니다.
    3. CGI 프로그램의 표준 입력으로 데이터를 넘겨주기 위해
        pipe 의 write buffer 에 데이터를 입력하는 이벤트
  - pipe read/write buffer 에 대해 등록하는 이유는
    pipe 의 최대 버퍼 크기 제한이 있어서
    한 번에 데이터를 읽고 쓰는 것이 불가능 하기 때문입니다.
*/
void Server::addCgiRequestEvent(struct kevent *current_event,
                                t_event_udata *current_udata,
                                struct Request &request,
                                struct Response &response)
{
  t_event_udata *read_pipe_udata;
  t_event_udata *timeout_udata;

  read_pipe_udata = createUdata(PIPE, current_event, current_udata, response);
  printf("[addCgiRequestEvent] read_pipe_udata: %p\n", read_pipe_udata); // TODO
  timeout_udata = createUdata(PROCESS, current_event, current_udata, response);
  printf("[addCgiRequestEvent] timeout_udata: %p\n", timeout_udata);  // TODO

  read_pipe_udata->m_other_udata = timeout_udata;
  timeout_udata->m_other_udata = read_pipe_udata;

  fcntl(response.read_pipe_fd, F_SETFL, O_NONBLOCK);
  addEventToChangeList(m_kqueue.change_list, response.read_pipe_fd, EVFILT_READ,
                       EV_ADD | EV_ENABLE, 0, 0, read_pipe_udata);
  addEventToChangeList(m_kqueue.change_list, response.cgi_child_pid,
                       EVFILT_TIMER, EV_ADD | EV_ONESHOT, NOTE_SECONDS,
                       DEFAULT_TIMEOUT_SECOND, timeout_udata);
  if (request.method == "POST")
  {
    t_event_udata *write_pipe_udata;

    write_pipe_udata =
        createUdata(PROCESS, current_event, current_udata, response);
    printf("[addCgiRequestEvent] write_pipe_udata: %p\n", write_pipe_udata);

    fcntl(response.write_pipe_fd, F_SETFL, O_NONBLOCK);
    addEventToChangeList(m_kqueue.change_list, response.write_pipe_fd,
                         EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0,
                         write_pipe_udata);
  }
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