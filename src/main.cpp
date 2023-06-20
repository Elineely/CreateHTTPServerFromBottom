#include "Config.hpp"
#include "Log.hpp"
#include "Server.hpp"

int main(int argc, char **argv)
{
  if (argc > 2)
  {
    Log::print(ERROR, "Usage: [./cute_webserv config_file]");
    exit(EXIT_FAILURE);
  }

  Config server_conf(argc == 2 ? argv[1] : DEFAULT_SERVER_FILE,
                     SERVER_CONTENT_FILE);
  Server server(server_conf);

  server.start();
}
