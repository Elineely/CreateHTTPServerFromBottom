#include <iostream>

#include "./Config.hpp"
#include "./PathFinder.hpp"
#include "./Request.hpp"
#include "./utils.hpp"

using namespace std;
int main(void)
{
  Config a("./server.conf", "./server_content.conf");
  a.showServerConf();
  Request b;
  Response c;
  b.uri = "/";
  cout << "start!" << endl;
  cout << b.uri << endl;
  PathFinder finder(b, a.get_m_server_conf()[9001], c);
  // cout << (a.get_m_server_conf())[0].locations.begin()->first << endl;
  return (0);
}