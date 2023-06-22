#include "Log.hpp"
#include "Server.hpp"

int Server::clientReadAccept(struct kevent *current_event)
{
  int client_sock;
  int client_addr_size;
  struct sockaddr_in client_addr;

  client_sock = accept(current_event->ident, (struct sockaddr *)&client_addr,
                       reinterpret_cast<socklen_t *>(&client_addr_size));
  if (client_sock == -1)
  {
    Log::print(ERROR, "Failed to accept client socket (strerror: %s)", strerror(errno));
    return (-1);
  }

  struct linger linger;

  linger.l_onoff = 1;
  linger.l_linger = 0;
  setsockopt(client_sock, SOL_SOCKET, SO_LINGER, &linger, sizeof(linger));
  return (client_sock);
}
