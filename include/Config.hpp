#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <algorithm>
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <vector>

#include "utils.hpp"

typedef std::pair<std::string, std::vector<std::string> >
    pair_string_string_type;
typedef std::map<std::string, std::vector<std::string> > map_string_string;
typedef std::pair<std::vector<std::string>, map_string_string> config_type;
typedef std::map<std::string, config_type> config_map;
typedef std::pair<std::string, config_type> config_map_type;

class Config
{
 private:
  std::string m_file_name;
  config_map m_server_conf;
  map_string_string m_null_map;

 private:
  config_map get_parse_brace(std::ifstream &file);
  map_string_string expend_key_brace(std::ifstream &file);
  config_map get_parse_brackat(std::ifstream &file);

 public:
  Config();
  Config(std::string file_name);
  Config(const Config &other);
  Config &operator=(const Config &other);
  ~Config();

  int getServerPort();
  void showServerConf();
};

#endif
