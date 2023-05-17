#include "Config.hpp"

void set_vaild_content_list(std::ifstream &content_file,
                            content_list_type &vaild_content_list)
{
  std::string read_line;
  int i = 0;
  while (getline(content_file, read_line, '\n'))
  {
    std::vector<std::string> split_line = ft_config_split(read_line);
    vaild_content_list.insert(std::pair<std::string, int>(split_line[0], i));
    ++i;
  }
}

Config::Config(std::string config_file_name,
               std::string config_content_file_name)
{
  std::ifstream config_file(config_file_name);
  std::ifstream content_file(config_content_file_name);
  content_list_type vaild_content_list;

  std::string read_line;
  int i = 0;

  if (content_file.fail())
  {
    ft_error(0, "ERROR: content file open fail", 1);
  }
  ft_process_print("config_content file open Success!!");
  set_vaild_content_list(content_file, vaild_content_list);
  ft_process_print("config_content file set Success!!");

  if (config_file.fail())
  {
    ft_error(0, "ERROR: config file open fail", 1);
  }
  ft_process_print("Config file open Success!!");
  set_m_server_conf(config_file, vaild_content_list);
  ft_process_print("Config file set Success!!");
}

Config::Config() { std::cout << "Config Construct call" << std::endl; }

Config::Config(const Config &other)
{
  std::cout << "Config Constructor Call" << std::endl;
  m_server_conf = other.m_server_conf;
  // m_null_map = other.m_null_map;
}

Config &Config::operator=(const Config &other)
{
  if (this == &other) return *this;
  std::cout << "Config Assignment Operator Call" << std::endl;
  m_server_conf = other.m_server_conf;
  // m_null_map = other.m_null_map;
  return *this;
}

Config::~Config() {}
