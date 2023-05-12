#ifndef CONFIG
# define CONFIG

#include <map>
#include <vector>
#include <sstream>
#include <fstream>
#include <fstream>
#include <iostream>
#include <algorithm>

typedef std::pair<std::string, std::vector<std::string> > pair_string_string_type;
typedef std::map<std::string, std::vector<std::string> > map_string_string;
typedef std::pair<std::vector<std::string>, map_string_string > config_type;
typedef std::map<std::string, config_type> config_map;
typedef std::pair<std::string, config_type> config_map_type;

class Config
{
    private:
        std::string fileName;
        config_map serverConf;
        map_string_string nullMap;

    private:
        config_map                  get_parse_brace(std::ifstream &file, int &currentLine);
        map_string_string           expend_key_brace(std::ifstream &file, int &currentLine);
        config_map                  get_parse_brackat(std::ifstream &file, int &currentLine);
        std::vector<std::string>    parse_split(std::string readLine, char delimiter);

    public:
        Config();
        Config(std::string fileName);
        Config(const Config& other);
		Config& operator=(const Config& a);
		~Config();
        
        int getServerPort();
        void showServerConf();

};

#endif  