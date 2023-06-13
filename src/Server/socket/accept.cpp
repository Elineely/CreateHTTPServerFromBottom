#include "Server.hpp"
#include "Log.hpp"

int Server::clientReadAccept(struct kevent *current_event)
{
  int client_sock;
  int client_addr_size;
  struct sockaddr_in client_addr;

  client_sock = accept(current_event->ident, (struct sockaddr *)&client_addr,
                       reinterpret_cast<socklen_t *>(&client_addr_size));
  if (client_sock == -1)
  {
    LOG_ERROR("Failed to accept client socket (strerror: %s)", strerror(errno));
    return (-1);
  }
  LOG_INFO("🌵 Client Socket fd %d is created 🌵", client_sock);
  return (client_sock);
}