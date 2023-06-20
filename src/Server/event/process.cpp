#include "HttpProcessor.hpp"
#include "Log.hpp"
#include "ResponseGenerator.hpp"
#include "Server.hpp"
#include "ServerFinder.hpp"
#include "utils.hpp"

t_event_udata *Server::createUdata(e_event_type type,
                                   struct kevent *current_event,
                                   t_event_udata *current_udata,
                                   struct Response &response)
{
  Request *new_request;
  Response *new_response;
  t_event_udata *udata;

  new_request = new Request(*current_udata->m_request);
  m_udata_map[current_udata->m_client_sock].push_back(new_request);
  new_response = new Response(response);
  m_udata_map[current_udata->m_client_sock].push_back(new_response);

  udata = new t_event_udata(type, current_udata->m_servers, new_request,
                            new_response);
  m_udata_map[current_udata->m_client_sock].push_back(udata);

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
  t_event_udata *read_pipe_udata = NULL;
  t_event_udata *timeout_udata = NULL;
  t_event_udata *write_pipe_udata = NULL;

  if (request.method == "POST")
  {
    struct Response new_response;

    write_pipe_udata =
        createUdata(PIPE, current_event, current_udata, new_response);

    fcntl(response.write_pipe_fd, F_SETFL, O_NONBLOCK);
    addEventToChangeList(m_kqueue.change_list, response.write_pipe_fd,
                         EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0,
                         write_pipe_udata);
  }

  read_pipe_udata = createUdata(PIPE, current_event, current_udata, response);
  timeout_udata = createUdata(PROCESS, current_event, current_udata, response);

  read_pipe_udata->m_other_udata = timeout_udata;
  read_pipe_udata->m_write_udata = write_pipe_udata;
  timeout_udata->m_other_udata = read_pipe_udata;
  timeout_udata->m_write_udata = write_pipe_udata;

  fcntl(response.read_pipe_fd, F_SETFL, O_NONBLOCK);
  addEventToChangeList(m_kqueue.change_list, response.read_pipe_fd, EVFILT_READ,
                       EV_ADD | EV_ENABLE, 0, 0, read_pipe_udata);
  addEventToChangeList(m_kqueue.change_list, response.cgi_child_pid,
                       EVFILT_TIMER, EV_ADD | EV_ONESHOT, NOTE_SECONDS,
                       DEFAULT_TIMEOUT_SECOND, timeout_udata);
  return ;
}

void Server::cgiProcessTimeoutEvent(struct kevent *current_event)
{
  std::vector<char> response_message;
  t_event_udata *current_udata;
  t_event_udata *udata;

  current_udata = static_cast<t_event_udata *>(current_event->udata);

  close(current_udata->m_read_pipe_fd);
  close(current_udata->m_write_pipe_fd);
  kill(current_event->ident, SIGTERM);
  waitpid(current_event->ident, NULL, 0);

  current_udata->m_response->is_cgi_timeout = true;
  ResponseGenerator not_ok(*current_udata->m_request,
                           *current_udata->m_response);
  response_message = not_ok.generateResponseMessage();
  udata =
      new t_event_udata(CLIENT, current_udata->m_servers,
                        current_udata->m_request, current_udata->m_response);
  m_udata_map[current_udata->m_client_sock].push_back(udata);
                        
  udata->m_response_write.message = response_message;
  udata->m_response_write.offset = 0;
  udata->m_response_write.length = response_message.size();

  Log::printRequestResult(current_udata);

  addEventToChangeList(m_kqueue.change_list, current_udata->m_client_sock,
                       EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, udata);
  for (size_t i = 0; i < current_udata->m_other_udata->m_read_buffer.size(); ++i)
  {
    delete current_udata->m_other_udata->m_read_buffer[i];
  }
  if (current_udata->m_write_udata != NULL)
  {
    ft_delete(current_udata->m_write_udata->m_request, current_udata->m_client_sock);
    ft_delete(current_udata->m_write_udata->m_response, current_udata->m_client_sock);
    ft_delete(current_udata->m_write_udata, current_udata->m_client_sock);
  }
  ft_delete(current_udata->m_other_udata->m_request, current_udata->m_client_sock);
  ft_delete(current_udata->m_other_udata->m_response, current_udata->m_client_sock);
  ft_delete(current_udata->m_other_udata, current_udata->m_client_sock);
  ft_delete(current_udata, current_udata->m_client_sock);
}
