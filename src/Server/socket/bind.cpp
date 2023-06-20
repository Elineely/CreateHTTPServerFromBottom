#include "Server.hpp"

void Server::startBind(std::vector<t_multi_server> &servers)
{
  int option = 1;
  for (unsigned long i = 0; i < servers.size(); ++i)
  {
    setsockopt(servers[i].server_sock, SOL_SOCKET, SO_REUSEADDR, &option,
               sizeof(option));
    if (bind(servers[i].server_sock, (struct sockaddr *)&servers[i].serv_addr,
             sizeof(servers[i].serv_addr)) == -1)
    {
      ft_error_exit(EXIT_FAILURE, strerror(errno));
    }
  }
}
