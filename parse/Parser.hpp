#ifndef PARSER
# define PARSER

#include <map>
#include <vector>
#include <sstream>
#include <fstream>
#include <fstream>
#include <iostream>
#include <algorithm>

typedef std::pair<std::string, std::string> pair_string_string;
typedef std::map<std::string, std::string> map_string_string;
typedef std::pair<std::string, map_string_string > parser_type;
typedef std::map<std::string, parser_type> parser_map;
typedef std::pair<std::string, parser_type> parser_map_type;

class Parser
{
    private:
        std::string fileName;
        parser_map serverConf;
        map_string_string nullMap;

    private:
        Parser();
        parser_map                  get_parse_brace(std::ifstream &file, int &currentLine);
        map_string_string           get_parse_brace2(std::ifstream &file, int &currentLine);
        parser_map                  get_parse_brackat(std::ifstream &file, int &currentLine);
        std::vector<std::string>    parse_split(std::string readLine, char delimiter);

    public:
        Parser(std::string fileName);
        Parser(const Parser& a);
		Parser& operator=(const Parser& a);
		~Parser();
        
        void showServerConf();

};

#endif 