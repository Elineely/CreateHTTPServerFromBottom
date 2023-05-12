#include "Config.hpp"

int Config::getServerPort()
{
	for (config_map::iterator iter=serverConf.begin(); iter != serverConf.end(); ++iter)
	{
		std::cout << iter->first << std::endl;
	}
	config_map::iterator iter = serverConf.find("port");
	if (iter != serverConf.end()) {
			return atoi(iter->second.first[0].c_str());
    } else {
        std::cout << "Key not found" << std::endl;
    }
	return 0;
}
