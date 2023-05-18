#include "Config.hpp"
#include "Server.hpp"

int main(int argc, char **argv)
{
  std::cout << "wellcome Cute webserv!!" << std::endl;

  Config server_conf(argc == 2 ? argv[1] : DEFAULT_SERVER_FILE,
                     SERVER_CONTENT_FILE);

  Server server(server_conf);
}