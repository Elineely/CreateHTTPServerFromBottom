#include "Server.hpp"
#include "Log.hpp"

void Server::serverErrorEvent(struct kevent *current_event)
{
  Log::error("🐛 Server socket error 🐛");
  disconnect_socket(current_event->ident);
}
