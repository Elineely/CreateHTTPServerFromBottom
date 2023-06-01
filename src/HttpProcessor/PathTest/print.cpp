#include "./Config.hpp"
// #include "../../include/Config.hpp"  //테스트용

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
    std::cout << "location : auto_index : " << (*iter).second.auto_index
              << std::endl;

    std::cout << "location : index : " << (*iter).second.index << std::endl;

    std::cout << "location : language : " << (*iter).second.language
              << std::endl;
    std::cout << "location : root : " << (*iter).second.root << std::endl;
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
