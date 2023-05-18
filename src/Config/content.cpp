#include "Config.hpp"

bool is_vaild_server_block_content(std::string split_content_line_content,
                                   content_list_type vaild_content_list)
{
  if (vaild_content_list.find(split_content_line_content) ==
      vaild_content_list.end())
    return true;
  return false;
}

bool is_content_count(int content_count, int valid_count)
{
  if (content_count != valid_count)
  {
    return true;
  }
  return false;
}

t_location Config::get_location_expend(std::ifstream &config_file,
                                       content_list_type vaild_content_list,
                                       int content_size)
{
  if (is_content_count(content_size, 3))
    ft_config_error("Error: non-vaild content in line:", current_line, 1);
  std::string read_line;
  t_location temp_conf_location;
  std::map<std::string, std::string> temp_conf_location_map;

  while (getline(config_file, read_line, '\n'))
  {
    std::vector<std::string> split_content_line = ft_config_split(read_line);
    if (split_content_line[0] == "}")
    {
      break;
    }
    if (is_vaild_server_block_content(split_content_line[0],
                                      vaild_content_list) ||
        split_content_line.size() >= 3)
      ft_config_error("Error: non-vaild content in line:", current_line, 1);
    temp_conf_location_map.insert(std::pair<std::string, std::string>(
        split_content_line[0], split_content_line[1]));
    current_line++;
  }
  temp_conf_location.auto_index = temp_conf_location_map["auto_index"];
  temp_conf_location.index = temp_conf_location_map["index"];
  temp_conf_location.language = temp_conf_location_map["language"];
  temp_conf_location.root = temp_conf_location_map["root"];

  return temp_conf_location;
}

t_server Config::get_parse_server_block(std::ifstream &file,
                                        content_list_type vaild_content_list)
{
  t_server server;
  std::string read_line;
  std::map<std::string, std::vector<std::string> > temp_conf;
  std::map<std::string, t_location> temp_conf_locations;

  while (getline(file, read_line, '\n'))
  {
    std::vector<std::string> split_content_line = ft_config_split(read_line);
    if (split_content_line[0] == "}")
    {
      break;
    }
    if (is_vaild_server_block_content(split_content_line[0],
                                      vaild_content_list))
      ft_config_error("Error: non-vaild content in line:", current_line, 1);
    if (split_content_line[0] == "location")
    {
      current_line++;
      temp_conf_locations.insert(std::pair<std::string, t_location>(
          split_content_line[1],
          get_location_expend(file, vaild_content_list,
                              split_content_line.size())));
    }
    else
    {
      std::string temp_content = split_content_line[0];
      split_content_line.assign(split_content_line.begin() + 1,
                                split_content_line.end());
      temp_conf.insert(
          pair_string_vector_string_type(temp_content, split_content_line));
    }
    current_line++;
  }
  server.server_name = temp_conf["server_name"];
  server.root = temp_conf["root"];
  server.index = temp_conf["index"];
  server.listen = temp_conf["listen"];
  server.max_body_size = temp_conf["max_body_size"];
  server.max_header_size = temp_conf["max_header_size"];
  server.locations = temp_conf_locations;

  return server;
}

void Config::set_m_server_conf(std::ifstream &config_file,
                               content_list_type vaild_content_list)
{
  std::string read_line;
  current_line = 1;
  while (getline(config_file, read_line, '\n'))
  {
    std::vector<std::string> split_content_line = ft_config_split(read_line);
    if (split_content_line.size() != 2 || split_content_line[0] != "server" ||
        split_content_line[1] != "{")
      ft_config_error("Error: non-vaild content in line:", current_line, 1);
    ++current_line;
    m_server_conf.push_back(
        get_parse_server_block(config_file, vaild_content_list));
  }
}