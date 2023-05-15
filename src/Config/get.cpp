#include "Config.hpp"

int Config::getServerPort()
{
  for (config_map::iterator iter = m_server_conf.begin();
       iter != m_server_conf.end(); ++iter)
  {
    std::cout << iter->first << std::endl;
  }
  config_map::iterator iter = m_server_conf.find("port");
  if (iter != m_server_conf.end())
  {
    return atoi(iter->second.first[0].c_str());
  }
  else
  {
    std::cout << "Key not found" << std::endl;
  }
  return 0;
}
