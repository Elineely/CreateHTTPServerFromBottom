#include "Config.hpp"
#include "Server.hpp"

int main(int argc, char **argv)
{
  if (argc > 2)
  {
    std::cerr << "Usage: [./cute_webserv config_file]" << std::endl;
    exit(EXIT_FAILURE);
  }
  std::cout << "🐈 Cute webserv! 🐈" << std::endl;

  Config server_conf(argc == 2 ? argv[1] : DEFAULT_SERVER_FILE,
                     SERVER_CONTENT_FILE);
  server_conf.showServerConf();
  Server server(server_conf);
}