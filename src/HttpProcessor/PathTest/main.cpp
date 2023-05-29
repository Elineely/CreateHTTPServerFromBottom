#include <iostream>

#include "./Config.hpp"
#include "./utils.hpp"

using namespace std;
int main(void)
{
  Config a("./server.conf", "./server_content.conf");
  a.showServerConf();
  // cout << (a.get_m_server_conf())[0].locations.begin()->first << endl;
  return (0);
}