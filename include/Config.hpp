#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <algorithm>
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <vector>

#include "utils.hpp"

// typedef std::pair<std::string, std::vector<std::string> >
//     pair_string_string_type;
// typedef std::map<std::string, std::vector<std::string> > map_string_string;
// typedef std::pair<std::vector<std::string>, map_string_string> config_vector;
// typedef std::map<std::string, config_vector> config_map;
// typedef std::pair<std::string, config_vector> config_map_type;

struct t_location
{
  std::string language;
  std::string root;
  std::string auto_index;
  std::string index;
};

struct t_server
{
  std::vector<std::string> server_name;
  std::vector<std::string> listen;
  std::vector<std::string> index;
  std::vector<std::string> root;
  std::vector<std::string> max_header_size;
  std::vector<std::string> max_body_size;
  std::map<std::string, t_location> locations;
};

typedef std::vector<t_server> config_vector;
typedef std::map<std::string, int> content_list_type;

class Config
{
 private:
  config_vector m_server_conf;
  int current_line;
  // config_map m_server_conf;
  // map_string_string m_null_map;

 private:
  t_server get_parse_server_block(std::ifstream &file,
                                  content_list_type vaild_content_list);
  // config_map get_parse_brace(std::ifstream &file);
  // map_string_string expend_key_brace(std::ifstream &file);
  // config_map get_parse_brackat(std::ifstream &file);

 public:
  Config();
  Config(std::string config_file_name, std::string config_content_file_name);
  Config(const Config &other);
  Config &operator=(const Config &other);
  ~Config();

  void set_m_server_conf(std::ifstream &config_file,
                         content_list_type vaild_content_list);
  int getServerPort();
  t_location get_location_expend(std::ifstream &config_file,
                                 content_list_type vaild_content_list);
  // void showServerConf();

  // void set_server_name_content(t_server &server,
  //                              std::vector<std::string> &content);
  // void set_listen_content(t_server &server, std::vector<std::string>
  // &content); void set_server_name_content(t_server &server,
  //                              std::vector<std::string> &content);
  // void set_index_content(t_server &server, std::vector<std::string>
  // &content); void set_root_content(t_server &server, std::vector<std::string>
  // &content); void set_max_header_size_content(t_server &server,
  //                                  std::vector<std::string> &content);
  // void set_max_body_size_content(t_server &server,
  //                                std::vector<std::string> &content);
  // void set_location_content(t_server &server,
  // std::vector<std::string> &content);
};

#endif
