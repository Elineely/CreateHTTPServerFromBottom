#include "Config.hpp"

Config::Config() { std::cout << "Config Construct call" << std::endl; }

Config::Config(std::string fileName) {
  std::ifstream file(fileName.c_str());

  if (file.fail()) {
    std::cout << "ERROR : can not find file!! " << std::endl;
    exit(1);
  }
  std::cout << "SUCCESS read file !!" << std::endl;

  std::string readLine;
  int currentLine = 1;
  serverConf = get_parse_brackat(file, currentLine);
}

// enter code after '{' token
config_map Config::get_parse_brace(std::ifstream &file, int &currentLine) {
  config_map map;
  std::string readLine;

  while (getline(file, readLine, '\n')) {
    if (readLine.find("}") != std::string::npos) return map;
    {
      std::vector<std::string> split_line = config_ft_split(readLine, ' ');
      std::vector<std::string> second_vector;
      for (int i = 1; i < split_line.size(); ++i)
        second_vector.push_back(split_line[i]);
      if (split_line[1].compare("{") == 0)
        map.insert(config_map_type(
            split_line[0],
            config_type(second_vector, expend_key_brace(file, currentLine))));
      map.insert(
          config_map_type(split_line[0], config_type(second_vector, nullMap)));
    }
    currentLine++;
  }
  return map;
}

map_string_string Config::expend_key_brace(std::ifstream &file,
                                           int &currentLine) {
  map_string_string map;
  std::string readLine;

  while (getline(file, readLine, '\n')) {
    if (readLine.find("}") != std::string::npos) return map;
    {
      std::vector<std::string> second_vector;
      std::vector<std::string> split_line = config_ft_split(readLine, ' ');
      for (int i = 1; i < split_line.size(); ++i)
        second_vector.push_back(split_line[i]);
      map.insert(pair_string_string_type(split_line[0], second_vector));
    }
    currentLine++;
  }
  return map;
}

config_map Config::get_parse_brackat(std::ifstream &file, int &currentLine) {
  std::string readLine;

  getline(file, readLine, '\n');   // pass brace {
  currentLine++;
  return (get_parse_brace(file, currentLine));
}

Config::Config(const Config &other) {
  std::cout << "Config Constructor Call" << std::endl;
  fileName = other.fileName;
  serverConf = other.serverConf;
  nullMap = other.nullMap;
}

Config &Config::operator=(const Config &other) {
  if (this == &other) return *this;
  std::cout << "Config Assignment Operator Call" << std::endl;
  fileName = other.fileName;
  serverConf = other.serverConf;
  nullMap = other.nullMap;
  return *this;
}

Config::~Config() {}
