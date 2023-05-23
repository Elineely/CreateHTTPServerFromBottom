#include "Server.hpp"

void Server::startBind(int server_sock, const struct sockaddr *server_addr)
{
  int option = 1;
  setsockopt(server_sock, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));
  if (bind(server_sock, server_addr, sizeof(*server_addr)) == -1)
  {
    ft_error_exit(1, strerror(errno));
  }
}