#include "Server.hpp"

void Server::setServers(Config server_conf,
                        std::vector<t_multi_server> &servers)
{
  config_vector server = server_conf.get_m_server_conf();

  for (config_vector::iterator iter = server.begin(); iter != server.end();
       ++iter)
  {
    for (int i = 0; i < iter->listen.size(); ++i)
    {
      t_multi_server server;
      server.server_port = atoi(iter->listen[i].c_str());
      server.config = *iter;
      servers.push_back(server);
    }
  }
}
