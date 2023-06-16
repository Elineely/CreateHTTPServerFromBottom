#include "Server.hpp"
#include "Log.hpp"

void Server::serverErrorEvent(struct kevent *current_event)
{
  LOG_INFO("🐛 Server socket error 🐛");
  disconnectSocket(current_event->ident);
}
