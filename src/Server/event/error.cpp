#include "Server.hpp"

void Server::serverErrorEvent(struct kevent *current_event)
{
  std::cout << "server socket error" << std::endl;
  disconnect_socket(current_event->ident);
}
