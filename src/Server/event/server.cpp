#include "Server.hpp"

#include "Log.hpp"
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

  int client_sock;
  Request *request;
  Response *response;
  t_event_udata *udata;

  client_sock = clientReadAccept(current_event);
  if (client_sock == -1)
  {
    return;
  }
  fcntl(client_sock, F_SETFL, O_NONBLOCK);

  try
  {
    request = new Request();

    response = new Response();

    udata =
        new t_event_udata(CLIENT, current_udata->m_servers, request, response);
    udata->m_client_sock = client_sock;
    addUdataMap(client_sock, udata);
  }
  catch (std::exception &e)
  {
    std::cout << e.what() << std::endl;
    exit(EXIT_FAILURE);
  }
  addEventToChangeList(m_kqueue.change_list, client_sock, EVFILT_READ,
                       EV_ADD | EV_ENABLE, 0, 0, udata);
}

/*
  [SUMMARY]
  - 서버 소켓에 오류가 발생한 이벤트를 처리하는 함수입니다.
*/
void Server::serverErrorEvent(struct kevent *current_event)
{
  t_event_udata *current_udata;

  current_udata = static_cast<t_event_udata *>(current_event->udata);
  disconnectSocket(current_event->ident);
  ft_delete(&current_udata);
}
