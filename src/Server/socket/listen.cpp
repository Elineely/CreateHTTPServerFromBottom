#include "Server.hpp"

void Server::startListen(std::vector<t_multi_server> &servers, int back_log)
{
  for (unsigned long i = 0; i < servers.size(); ++i)
  {
    if (listen(servers[i].server_sock, back_log) == -1)
    {
      ft_error_exit(EXIT_FAILURE, strerror(errno));
    }
  }
}
