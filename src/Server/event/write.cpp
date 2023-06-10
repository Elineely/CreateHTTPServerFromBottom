#include "Server.hpp"
#include "Log.hpp"

void Server::clientWriteEvent(struct kevent *current_event)
{
  LOG_INFO("✅ CLIENT WRITE EVENT ✅");

  t_event_udata *udata;
  t_event_udata *new_udata;
  t_event_udata *read_udata;
  t_response_write *response_write;
  char *message;

  udata = static_cast<t_event_udata *>(current_event->udata);
  response_write = &udata->m_response_write;
  message = &response_write->message[0];
  // write(1, message, response_write->length);
  int send_byte = 0;
  send_byte = send(current_event->ident, message + response_write->offset,
                   response_write->length - response_write->offset, 0);
  response_write->offset += send_byte;
  if (response_write->length > response_write->offset)
  {
    return;
  }
  AddEventToChangeList(m_kqueue.change_list, current_event->ident, EVFILT_WRITE,
                       EV_DELETE, 0, 0, NULL);
  delete udata;
}
