#include "Server.hpp"
#include "Log.hpp"

void Server::cgiProcessTimeoutEvent(struct kevent *current_event)
{
  LOG_INFO("⌛️ CGI PROCESS TIMEOUT EVENT ⌛️");
  std::vector<char> response_message;

  t_event_udata *current_udata =
      static_cast<t_event_udata *>(current_event->udata);

  close(current_udata->m_pipe_read_fd);
  int result = kill(current_event->ident, SIGTERM);
  waitpid(current_event->ident, NULL, 0);

  ResponseGenerator not_ok(current_udata->m_parser.get_request(), current_udata->m_response);
  response_message = not_ok.generateResponseMessage();
  t_event_udata *udata =
      new t_event_udata(CLIENT, response_message, response_message.size());

  write(1, &response_message[0], response_message.size());
  AddEventToChangeList(m_kqueue.change_list, current_udata->m_client_sock, EVFILT_WRITE,
                       EV_ADD | EV_ENABLE, 0, 0, udata);
  delete current_udata->m_other_udata;
  delete current_udata;
}