#include "Config.hpp"
#include "Server.hpp"

int main()
{
  std::cout << "wellcome Cute webserv!!" << std::endl;

  Config server_conf("/server.conf");

  Server server(server_conf);
}