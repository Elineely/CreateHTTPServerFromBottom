#include "Server.hpp"
#include "Log.hpp"

void Server::clientWriteEvent(struct kevent *current_event)
{
  Log::info("✅ CLIENT WRITE EVENT ✅");

  t_event_udata *udata;
  t_response_write *response;

  udata = static_cast<t_event_udata *>(current_event->udata);
  response = &udata->m_response;
  Log::debug("response->message: %s", response->message);
  int send_byte = 0;
  send_byte = send(current_event->ident, response->message + response->offset,
                   response->length - response->offset, 0);
  response->offset += send_byte;
  if (response->length > response->offset)
  {
    return;
  }
  AddEventToChangeList(m_kqueue.change_list, current_event->ident, EVFILT_WRITE,
                       EV_DELETE, 0, 0, NULL);
  delete udata;
}
