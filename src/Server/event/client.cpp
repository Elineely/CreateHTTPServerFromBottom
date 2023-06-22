#include "HttpProcessor.hpp"
#include "Log.hpp"
#include "ResponseGenerator.hpp"
#include "Server.hpp"
#include "ServerFinder.hpp"

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
  else if (recv_size == -1)
  {
    ft_error_exit(EXIT_FAILURE,"recv system call error");
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
    Log::print(INFO, "💥 Client socket(fd: %d) will be close 💥", current_event->ident);

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

  try
  {
    current_udata->m_request = new Request();
    current_udata->m_response = new Response();
  }
  catch(const std::exception &e)
  {
    exit(EXIT_FAILURE);
  }
  current_udata->m_parser = new_parser;
}

/*
  [SUMMARY]
  - 클라이언트 소켓의 EVFILT_WRITE 이벤트를 처리하는 함수입니다.
*/
void Server::clientWriteEvent(struct kevent *current_event)
{
  t_event_udata *current_udata;
  t_response_write *response_write;
  char *message;
  ssize_t send_byte;

  current_udata = static_cast<t_event_udata *>(current_event->udata);
  response_write = &current_udata->m_response_write;
  message = &response_write->message[0];
  send_byte = send(current_event->ident, message + response_write->offset,
                   response_write->length - response_write->offset, 0);
  response_write->offset += send_byte;
  if (response_write->length > response_write->offset)
  {
    return;
  }
  addEventToChangeList(m_kqueue.change_list, current_event->ident, EVFILT_WRITE,
                       EV_DELETE, 0, 0, NULL);
  ft_delete(&(current_udata->m_request));
  ft_delete(&(current_udata->m_response));
  ft_delete(&current_udata);
}
