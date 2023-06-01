#include "Server.hpp"

void Server::setSocket(Config server_conf, std::vector<t_multi_server> &servers)
{
  for (int i = 0; i < servers.size(); ++i)
  {
    servers[i].server_sock = socket(PF_INET, SOCK_STREAM, 0);
    std::cout << "socket_fd :" << servers[i].server_sock
              << " port :" << servers[i].server_port << std::endl;
    if (servers[i].server_sock == -1)
    {
      ft_error_exit(1, strerror(errno));
    }
    servers[i].serv_addr.sin_family = AF_INET;
    servers[i].serv_addr.sin_port = htons(servers[i].server_port);
    servers[i].serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  }
}