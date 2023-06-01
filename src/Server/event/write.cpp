#include "Server.hpp"

void Server::clientWriteEvent(struct kevent *current_event)
{
  std::cout << "✅ CLIENT WRITE EVENT ✅" << std::endl;

  t_response_write *response;
  response = static_cast<t_response_write *>(current_event->udata);

  int send_byte = 0;
  send_byte = send(current_event->ident, response->message + response->offset,
                   response->length - response->offset, 0);
  response->offset += send_byte;
  if (response->length > response->offset)
  {
    return;
  }
  AddEventToChangeList(CLIENT, m_kqueue.change_list, current_event->ident,
                       EVFILT_WRITE, EV_DELETE, 0, 0, response);
  // free(response);
}
