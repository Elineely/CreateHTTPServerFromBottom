#include "Config.hpp"

#include "Log.hpp"

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

  if (content_file.fail())
  {
    LOG_ERROR("Failed to open %s", config_content_file_name.c_str());
    exit(EXIT_FAILURE);
  }
  LOG_INFO("Successfully open %s", config_content_file_name.c_str());
  set_vaild_content_list(content_file, vaild_content_list);
  LOG_INFO("Successfully validate %s", config_content_file_name.c_str());

  if (config_file.fail())
  {
    LOG_ERROR("Failed to open %s", config_file_name.c_str());
    exit(EXIT_FAILURE);
  }
  LOG_INFO("Successfully open %s", config_file_name.c_str());
  set_m_server_conf(config_file, config_file_name, vaild_content_list);
  LOG_INFO("Successfully validate %s", config_file_name.c_str());
}

Config::Config() { std::cout << "Config Construct call" << std::endl; }

Config::Config(const Config &other)
{
  std::cout << "Config Constructor Call" << std::endl;
  m_server_conf = other.m_server_conf;
}

Config &Config::operator=(const Config &other)
{
  if (this == &other) return *this;
  std::cout << "Config Assignment Operator Call" << std::endl;
  m_server_conf = other.m_server_conf;
  return *this;
}

Config::~Config() {}

config_vector Config::get_m_server_conf() const { return m_server_conf; }