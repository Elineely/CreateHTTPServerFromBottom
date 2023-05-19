#include "Server.hpp"

void Server::startListen(int server_sock, int back_log)
{
  if (listen(server_sock, back_log) == -1)
  {
    ft_error_exit(1, strerror(errno));
  }
}