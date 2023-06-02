#include "Server.hpp"

void Server::cgiProcessTimeoutEvent(struct kevent *current_event)
{
  std::cout << "⌛️ CGI PROCESS TIMEOUT EVENT ⌛️" << std::endl;
  t_event_udata *current_udata =
      static_cast<t_event_udata *>(current_event->udata);

  close(current_udata->m_pipe_read_fd);
  int result = kill(current_event->ident, SIGTERM);
  waitpid(current_event->ident, NULL, 0);

  char *message = getHttpCharMessages();
  t_event_udata *udata = new t_event_udata(CLIENT, message, ft_strlen(message));

  AddEventToChangeList(m_kqueue.change_list, udata->m_client_sock, EVFILT_WRITE,
                       EV_ADD | EV_ENABLE, 0, 0, udata);
  delete current_udata;
}