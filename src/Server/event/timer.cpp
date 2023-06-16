#include "Server.hpp"
#include "Log.hpp"

void Server::cgiProcessTimeoutEvent(struct kevent *current_event)
{
  LOG_INFO("⌛️ CGI PROCESS TIMEOUT EVENT ⌛️");
  std::vector<char> response_message;

  t_event_udata *current_udata =
      static_cast<t_event_udata *>(current_event->udata);

  close(current_udata->m_read_pipe_fd);
  close(current_udata->m_write_pipe_fd);
  int result = kill(current_event->ident, SIGTERM);
  waitpid(current_event->ident, NULL, 0);

  ResponseGenerator not_ok(*current_udata->m_request, *current_udata->m_response);
  response_message = not_ok.generateResponseMessage();
  t_event_udata *udata =
      new t_event_udata(CLIENT, current_udata->m_server, current_udata->m_request, current_udata->m_response);

  write(1, &response_message[0], response_message.size());
  addEventToChangeList(m_kqueue.change_list, current_udata->m_client_sock, EVFILT_WRITE,
                       EV_ADD | EV_ENABLE, 0, 0, udata);
  delete current_udata->m_other_udata;
  delete current_udata;
}