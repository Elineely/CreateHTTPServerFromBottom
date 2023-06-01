#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <algorithm>
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <vector>

#include "./utils.hpp"

struct t_location
{
  std::string language;
  std::string root;
  std::string auto_index;
  std::string index;
  std::string ourcgi_pass;
  std::string ourcgi_index;
  std::string uploaded_path;
  std::string accepted_method;
  std::string redirection;
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
typedef std::pair<std::string, std::vector<std::string> >
    pair_string_vector_string_type;

class Config
{
 private:
  int current_line;
  config_vector m_server_conf;

 public:
 private:
  t_server get_parse_server_block(std::ifstream &file,
                                  content_list_type vaild_content_list);

 public:
  Config();
  Config(std::string config_file_name, std::string config_content_file_name);
  Config(const Config &other);
  Config &operator=(const Config &other);
  ~Config();

  void set_m_server_conf(std::ifstream &config_file,
                         content_list_type vaild_content_list);
  config_vector get_m_server_conf() const;
  t_location get_location_expend(std::ifstream &config_file,
                                 content_list_type vaild_content_list,
                                 int content_size);
  void showServerConf();
};

#endif
