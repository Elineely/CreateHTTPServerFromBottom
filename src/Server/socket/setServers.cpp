#include "Server.hpp"

void Server::setServers(const Config& server_conf,
                        std::vector<t_multi_server> &servers)
{
  config_vector server = server_conf.get_m_server_conf();
  std::map<int, int> port_map;

  for (config_vector::iterator iter = server.begin(); iter != server.end();
       ++iter)
  {
    for (size_t i = 0; i < iter->listen.size(); ++i)
    {
      int port = atoi(iter->listen[i].c_str());
      if (port_map.find(port) != port_map.end())
      {
        ft_error_exit(EXIT_FAILURE, "Duplicated port is not allowed.");
      }
      t_multi_server server;
      server.server_port = port;
      servers.push_back(server);
      port_map.insert(std::make_pair(port, 0));
    }
  }
}
