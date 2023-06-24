#include "Config.hpp"
#include "Log.hpp"

void showIterVector(std::vector<std::string> vector)
{
  for (std::vector<std::string>::iterator iter = vector.begin();
       iter != vector.end(); ++iter)
  {
    std::cout << iter[0] << " ";
  }
}

void showIterMap(std::map<std::string, t_location> location)
{
  for (std::map<std::string, t_location>::iterator iter = location.begin();
       iter != location.end(); ++iter)
  {
    Log::print(DEBUG, "location auto_index: %s", (*iter).second.auto_index.c_str());
    Log::print(DEBUG, "location index: %s", (*iter).second.index.c_str());
    Log::print(DEBUG, "location language: %s", (*iter).second.language.c_str());
    Log::print(DEBUG, "location root: %s", (*iter).second.root.c_str());
  }
}

void Config::showServerConf()
{
  showIterMap(m_server_conf[0].locations);
  showIterVector(m_server_conf[0].server_name);
  std::cout << std::endl;
  showIterVector(m_server_conf[0].root);
  std::cout << std::endl;
  showIterVector(m_server_conf[0].index);
  std::cout << std::endl;
  showIterVector(m_server_conf[0].listen);
  std::cout << std::endl;
  showIterVector(m_server_conf[0].max_body_size);
  std::cout << std::endl;
  showIterVector(m_server_conf[0].max_header_size);
}
