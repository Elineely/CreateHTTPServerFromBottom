#include "Log.hpp"
#include "Server.hpp"
#include <utility>

void Server::setSocket(std::vector<t_multi_server>& servers)
{
  for (size_t i = 0; i < servers.size(); ++i)
  {
    servers[i].server_sock = socket(PF_INET, SOCK_STREAM, 0);

    Log::print(INFO, "Server port number: %d (socket fd: %d)", servers[i].server_port,
             servers[i].server_sock);

    if (servers[i].server_port <= 0)
    {
      ft_error_exit(1, strerror(errno));
    }

    if (servers[i].server_sock == -1)
    {
      ft_error_exit(1, strerror(errno));
    }
    servers[i].serv_addr.sin_family = AF_INET;
    servers[i].serv_addr.sin_port = htons(servers[i].server_port);
    servers[i].serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  }
}
