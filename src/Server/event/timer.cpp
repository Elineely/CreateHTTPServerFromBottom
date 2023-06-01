#include "Server.hpp"

void Server::cgiProcessTimeoutEvent(struct kevent *current_event)
{
  std::cout << "⌛️ CGI PROCESS TIMEOUT EVENT ⌛️" << std::endl;
  t_event_udata *current_udata =
      static_cast<t_event_udata *>(current_event->udata);
  // pipe close
  close(current_udata->m_pipe_read_fd);
  int result = kill(current_event->ident, SIGTERM);
  waitpid(current_event->ident, NULL, 0);

  // error 메시지로 변경 후 전달
  char *message = getHttpCharMessages();
  // char message[] =
  //     "HTTP/1.1 200 OK\r\nContent-type: text/plain\r\nContent-length: "
  //     "5\r\n\r\nhello";  // 인자로 response 전달
  t_event_udata *udata = new t_event_udata(CLIENT, message, ft_strlen(message));
  AddEventToChangeList(m_kqueue.change_list, udata->m_client_sock, EVFILT_WRITE,
                       EV_ADD | EV_ENABLE, 0, 0, udata);
}