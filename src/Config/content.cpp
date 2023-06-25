#include <fcntl.h>
#include <unistd.h>

#include "Config.hpp"
#include "Log.hpp"

#define BUF_SIZE 1024

bool isVaildServerBlockContent(std::string split_content_line_content,
                               content_list_type vaild_content_list)
{
  if (vaild_content_list.find(split_content_line_content) ==
      vaild_content_list.end())
    return true;
  return false;
}

bool isContentCount(int content_count, int valid_count)
{
  if (content_count != valid_count)
  {
    return true;
  }
  return false;
}

bool Config::isVaildServerBlock(t_server &server)
{
  if (server.listen.size() == 0)
  {
    Log::print(ERROR, "Invaild Server Content, does not have listen content");
    return 1;
  }
  if (server.max_body_size.size() == 0)
  {
    Log::print(ERROR,
               "Invaild Server Content, does not have max_body_size content");
    return 1;
  }
  else if(std::atol(server.max_body_size[0].c_str()) <= 0)
  { 
    Log::print(ERROR,
               "Invaild Server Content, has INVALID max_body_size content");
    return 1;
  }
  return 0;
}

bool Config::isVaildLocationBlock(t_location &location)
{
  if (location.root == "")
  {
    Log::print(ERROR, "Invaild Location, does not have root content");
    return 1;
  }
  if (location.accepted_method == "")
  {
    Log::print(ERROR,
               "Invaild Location, does not have accepted_method content");
    return 1;
  }
  return 0;
}

t_location Config::get_location_expand(std::ifstream &config_file,
                                       std::string config_file_name,
                                       content_list_type vaild_content_list,
                                       int content_size,
                                       std::string server_max_size)
{
  if (isContentCount(content_size, 3))
  {
    Log::print(ERROR,
               "[get_location_expand:isContentCount] Invalid content  in %s",
               config_file_name.c_str());
    exit(EXIT_FAILURE);
  }
  std::string read_line;
  t_location temp_location;
  std::map<std::string, std::string> temp_location_map;

  while (getline(config_file, read_line, '\n'))
  {
    std::vector<std::string> split_content_line = ft_config_split(read_line);
    if (split_content_line[0] == "}")
    {
      break;
    }
    if (isVaildServerBlockContent(split_content_line[0], vaild_content_list) ||
        split_content_line.size() >= 3 || split_content_line.size() == 1)
    {
      Log::print(ERROR,
                 "[get_location_expand:isVaildServerBlockContent]Invalid "
                 "content  in %s",
                 config_file_name.c_str());
      exit(EXIT_FAILURE);
    }
    temp_location_map.insert(std::pair<std::string, std::string>(
        split_content_line[0], split_content_line[1]));
  }
  temp_location.auto_index = temp_location_map["auto_index"];
  temp_location.index = temp_location_map["index"];
  temp_location.language = temp_location_map["language"];
  temp_location.root = temp_location_map["root"];
  temp_location.ourcgi_pass = temp_location_map["ourcgi_pass"];
  temp_location.ourcgi_index = temp_location_map["ourcgi_index"];
  temp_location.uploaded_path = temp_location_map["uploaded_path"];
  temp_location.accepted_method = temp_location_map["accepted_method"];
  temp_location.redirection = temp_location_map["redirection"];
  temp_location.max_body_size = server_max_size;
  if (temp_location_map["max_body_size"] != "")
  {
    if (std::atol(temp_location_map["max_body_size"].c_str()) <= 0)
    { 
     Log::print(ERROR,
               "Invaild Location Content, has INVALID max_body_size content");
      exit(EXIT_FAILURE);
    }
    temp_location.max_body_size = temp_location_map["max_body_size"];
  }

  if (isVaildLocationBlock(temp_location))
  {
    exit(EXIT_FAILURE);
  }
  return temp_location;
}

void Config::openErrorPage(t_server &server)
{
  if (server.error_page.size() == 0)
  {
    return;
  }

  int file_fd;
  ssize_t read_byte;
  char buf[BUF_SIZE];
  std::string file_path;

  file_path = (server.root.size() == 0 ? "./data" : server.root[0]) + "/" +
              server.error_page[0];
  file_fd = open(file_path.c_str(), O_RDONLY, 0644);
  if (file_fd == -1)
  {
    return;
  }

  read_byte = read(file_fd, buf, BUF_SIZE);
  if (read_byte == -1)
  {
    ft_error_exit(EXIT_FAILURE, "failed to read default error page");
  }
  while (read_byte > 0)
  {
    for (ssize_t i = 0; i < read_byte; ++i)
    {
      server.error_page_vector.push_back(buf[i]);
    }
    read_byte = read(file_fd, buf, BUF_SIZE);
    if (read_byte == -1)
    {
      ft_error_exit(EXIT_FAILURE, "failed to read default error page");
    }
  }
  close(file_fd);
}

t_server Config::get_parse_server_block(std::ifstream &file,
                                        std::string config_file_name,
                                        content_list_type vaild_content_list)
{
  t_server server;
  std::string read_line;
  std::map<std::string, std::vector<std::string> > temp_conf;
  std::map<std::string, t_location> temp_locations;

  while (getline(file, read_line, '\n'))
  {
    std::vector<std::string> split_content_line = ft_config_split(read_line);
    if (split_content_line.size() == 0) continue;
    if (split_content_line[0] == "}")
    {
      break;
    }
    if (isVaildServerBlockContent(split_content_line[0], vaild_content_list))
    {
      Log::print(ERROR,
                 "[get_parse_server_block:isVaildServerBlockContent] Invalid "
                 "content  in %s",
                 config_file_name.c_str());
      exit(EXIT_FAILURE);
    }
    if (split_content_line[0] == "location")
    {
      std::string temp_string;
      if (temp_conf.find("max_body_size") == temp_conf.end())
        temp_string = "";
      else
        temp_string = temp_conf["max_body_size"][0];
      temp_locations.insert(std::pair<std::string, t_location>(
          split_content_line[1],
          get_location_expand(file, config_file_name, vaild_content_list,
                              split_content_line.size(), temp_string)));
    }
    else
    {
      std::string temp_content = split_content_line[0];
      split_content_line.assign(split_content_line.begin() + 1,
                                split_content_line.end());
      temp_conf.insert(
          pair_string_vector_string_type(temp_content, split_content_line));
    }
  }
  server.server_name = temp_conf["server_name"];
  server.root = temp_conf["root"];
  server.index = temp_conf["index"];
  server.listen = temp_conf["listen"];
  server.max_body_size = temp_conf["max_body_size"];
  server.max_header_size = temp_conf["max_header_size"];
  server.error_page = temp_conf["error_page"];
  server.locations = temp_locations;

  if (isVaildServerBlock(server))
  {
    exit(EXIT_FAILURE);
  }

  openErrorPage(server);

  return server;
}

void Config::set_m_server_conf(std::ifstream &config_file,
                               std::string config_file_name,
                               content_list_type vaild_content_list)
{
  std::string read_line;
  while (getline(config_file, read_line, '\n'))
  {
    std::vector<std::string> split_content_line = ft_config_split(read_line);
    if (split_content_line.size() != 2 || split_content_line[0] != "server" ||
        split_content_line[1] != "{")
    {
      Log::print(ERROR, "[set_m_server_conf] Invalid content  in %s",
                 config_file_name.c_str());
      exit(EXIT_FAILURE);
    }
    m_server_conf.push_back(get_parse_server_block(
        config_file, config_file_name, vaild_content_list));
  }
}
