#include "Server.hpp"

void Server::setSocket(Config server_conf)
{
  config_vector server = server_conf.get_m_server_conf();
  m_socket.server_sock = socket(PF_INET, SOCK_STREAM, 0);
  if (m_socket.server_sock == -1)
  {
    ft_error_exit(1, strerror(errno));
  }
  m_socket.serv_addr.sin_family = AF_INET;

  m_socket.serv_addr.sin_port = htons(atoi(server[0].listen[0].c_str()));
  m_socket.serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
}