#include "Config.hpp"
#include "Server.hpp"

int main()
{
  std::cout << "wellcome Cute webserv!!" << std::endl;

  // 실행프로그램 기준으로 상대 경로를 정의한다
  Config server_conf("./server.conf");

  Server server(server_conf);
}