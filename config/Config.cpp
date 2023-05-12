#include "Config.hpp"

Config::Config()
{
	std::cout << "Config Construct call" << std::endl;
}

Config::Config(std::string fileName)
{
	std::ifstream file(fileName.c_str());

	if (file.fail())
	{
		std::cout << "ERROR : can not find file!! " << std::endl;
		exit(1);
	}
	std::cout << "SUCCESS read file !!" << std::endl;

	std::string readLine;
	int currentLine = 1;
	serverConf = get_parse_brackat(file, currentLine);
}

// enter code after '{' token
config_map Config::get_parse_brace(std::ifstream &file, int &currentLine)
{
	config_map map;
	std::string readLine;

	while(getline(file, readLine, '\n'))
	{
		if (readLine.find("}") != std::string::npos || readLine.find("]") != std::string::npos)
		{
			std::cout << "meed } or ] in brace1 " << std::endl;
			return map;	
		}
		{
			std::vector<std::string> split_line = parse_split(readLine, ':');
			if (split_line.size() != 2)
			{
				std::cout << "not vaild format Line Number : " << currentLine << std::endl;
				break ;
			}
			split_line[0].erase(remove(split_line[0].begin(), split_line[0].end(), ' '), split_line[0].end());
			split_line[1].erase(remove(split_line[1].begin(), split_line[1].end(), ' '), split_line[0].end());
			if (split_line[1].compare("[") == 0)
				map.insert(config_map_type(split_line[0], config_type(split_line[1], get_parse_brace2(file, currentLine) )));
			map.insert(config_map_type(split_line[0], config_type(split_line[1], nullMap )));
			std::cout << "[" << split_line[0] << "] : [" << split_line[1] << "] Line : " << currentLine << std::endl;
		}
		currentLine++;
	}
	return map;
}

map_string_string Config::get_parse_brace2(std::ifstream &file, int &currentLine)
{
	map_string_string map;
	std::string readLine;

	getline(file, readLine, '\n');
	while(getline(file, readLine, '\n'))
	{
		if (readLine.find("}") != std::string::npos || readLine.find("]") != std::string::npos)
		{
			std::cout << "meed } or ] in brace2 " << std::endl;
			return map;	
		}
		{
			std::cout << "readLine : " << readLine << std::endl;
			std::vector<std::string> split_line = parse_split(readLine, ':');
			if (split_line.size() != 2)
			{
				std::cout << "not vaild format Line Number : " << currentLine << std::endl;
			}
			split_line[0].erase(remove(split_line[0].begin(), split_line[0].end(), ' '), split_line[0].end());
			split_line[1].erase(remove(split_line[1].begin(), split_line[1].end(), ' '), split_line[0].end());
			map.insert(pair_string_string(split_line[0], split_line[1]));
		}
		currentLine++;
	}
	return map;
}

config_map Config::get_parse_brackat(std::ifstream &file, int &currentLine)
{
	std::string readLine;

	getline(file, readLine, '\n'); // pass brace
	currentLine++;
	return (get_parse_brace(file, currentLine));
}

std::vector<std::string> Config::parse_split(std::string readLine, char delimiter)
{
	std::string buffer;
	std::istringstream iss(readLine); 
	std::vector<std::string> split_readLine;

	while (getline(iss, buffer, delimiter)) {
        split_readLine.push_back(buffer);               // 절삭된 문자열을 vector에 저장
    }
 
	return split_readLine;
}

void showIterMap(map_string_string map)
{
	for (map_string_string::iterator iter = map.begin(); iter != map.end(); ++iter)
	{
		std::cout << "		" << iter->first << " : " << iter->second << std::endl;
	}
}

void Config::showServerConf()
{
	for(config_map::iterator iter = serverConf.begin(); iter != serverConf.end(); ++iter)
	{
		std::cout << iter->first << " : " << iter->second.first << std::endl;
		if (iter->second.second != nullMap)
		{
			showIterMap(iter->second.second);
			std::cout << "	]" << std::endl;
		}
	}
}

Config::Config(const Config& other)
{
    std::cout << "Config Constructor Call" << std::endl;
	*this = other;
}

Config& Config::operator=(const Config& other)
{
    if (this == &other)
        return *this;
    std::cout << "Config Assignment Operator Call" << std::endl;
    return *this;
}

Config::~Config()
{

}