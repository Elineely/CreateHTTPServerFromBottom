#include "Config.hpp"

int check_vaild_server_block_content(std::vector<std::string> split_line,
                                     content_list_type vaild_content_list)
{
  if (vaild_content_list.find(split_line[0]) == vaild_content_list.end())
    return 1;
  return 0;
}

t_location Config::get_location_expend(std::ifstream &config_file,
                                       content_list_type vaild_content_list)
{
  t_location location;
  std::string read_line;
  std::cout << "in here" << std::endl;
  while (getline(config_file, read_line, '\n'))
  {
    if (read_line.find("}") != std::string::npos)
    {
      return location;
    }
    {
      std::vector<std::string> split_line = ft_config_split(read_line);
      std::cout << split_line[0] << " " << split_line[1] << std::endl;
      if (check_vaild_server_block_content(split_line, vaild_content_list) ||
          split_line.size() >= 3)
        ft_config_error("Error: non-vaild content in line:", current_line, 1);

      if (split_line[0].compare("root") == 0)
      {
        location.root = split_line[1];
      }
      else if (split_line[0].compare("auto_index") == 0)
      {
        location.auto_index = split_line[1];
      }
      else if (split_line[0].compare("index") == 0)
      {
        location.index = split_line[1];
      }
      else
      {
        ft_config_error("Error: non-vaild content in line:", current_line, 1);
      }
    }
    current_line++;
  }
  return location;
}

t_server Config::get_parse_server_block(std::ifstream &file,
                                        content_list_type vaild_content_list)
{
  t_server server;
  std::string read_line;
  while (getline(file, read_line, '\n'))
  {
    if (read_line.find("}") != std::string::npos)
    {
      return server;
    }
    std::vector<std::string> split_line = ft_config_split(read_line);
    std::cout << split_line[0] << std::endl;
    if (check_vaild_server_block_content(split_line, vaild_content_list))
      ft_config_error("Error: non-vaild content in line:", current_line, 1);
    if (split_line[0].compare("location") == 0)
    {
      if (split_line.size() == 3)
      {
        current_line++;
        server.locations.insert(std::pair<std::string, t_location>(
            split_line[1], get_location_expend(file, vaild_content_list)));
      }
    }
    else if (split_line[0].compare("server_name") == 0)
    {
      server.server_name.assign(split_line.begin() + 1, split_line.end());
    }
    else if (split_line[0].compare("listen") == 0)
    {
      server.listen.assign(split_line.begin() + 1, split_line.end());
    }
    else if (split_line[0].compare("index") == 0)
    {
      server.index.assign(split_line.begin() + 1, split_line.end());
    }
    else if (split_line[0].compare("root") == 0)
    {
      server.root.assign(split_line.begin() + 1, split_line.end());
    }
    else if (split_line[0].compare("max_header_size") == 0)
    {
      server.max_header_size.assign(split_line.begin() + 1, split_line.end());
    }
    else if (split_line[0].compare("max_body_size") == 0)
    {
      server.max_body_size.assign(split_line.begin() + 1, split_line.end());
    }
    current_line++;
  }
  return server;
}

void Config::set_m_server_conf(std::ifstream &config_file,
                               content_list_type vaild_content_list)
{
  std::string read_line;
  current_line = 1;
  while (getline(config_file, read_line, '\n'))
  {
    std::vector<std::string> split_line = ft_config_split(read_line);
    std::cout << split_line.size() << " " << split_line[0].compare("server")
              << " " << split_line[1].compare("{") << std::endl;
    if (split_line.size() != 2 && split_line[0].compare("server") != 0 &&
        split_line[1].compare("{") != 0)
      ft_config_error("Error: non-vaild content in line:", current_line, 1);
    ++current_line;
    m_server_conf.push_back(
        get_parse_server_block(config_file, vaild_content_list));
  }
}

// void Config::set_server_name_content(t_server &server,
//                                      std::vector<std::string> &content)
// {
//   server.server_name = content;
// }

// void Config::set_listen_content(t_server &server,
//                                 std::vector<std::string> &content)
// {
//   server.listen = content;
// }

// void Config::set_server_name_content(t_server &server,
//                                      std::vector<std::string> &content)
// {
//   server.server_name = content;
// }

// void Config::set_index_content(t_server &server,
//                                std::vector<std::string> &content)
// {
//   server.index = content;
// }

// void Config::set_root_content(t_server &server,
//                               std::vector<std::string> &content)
// {
//   server.root = content;
// }

// void Config::set_max_header_size_content(t_server &server,
//                                          std::vector<std::string> &content)
// {
//   server.max_header_size = content;
// }

// void Config::set_max_body_size_content(t_server &server,
//                                        std::vector<std::string> &content)
// {
//   server.max_body_size = content;
// }

// void Config::set_location_content(t_server &server,
//                                   std::vector<std::string> &content)
// {
// }