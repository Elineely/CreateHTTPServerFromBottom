#include "/Users/surinlee/Desktop/42/5circle/our_webserv/include/PathFinder.hpp"
// #include "../../include/PathFinder.hpp"
// #include "PathFinder.hpp"

PathFinder::PathFinder() {}

PathFinder::~PathFinder() {}

PathFinder::PathFinder(Request requset_data, t_server server_data,
                       Response& response_data)
{
  std::string locationBlock = requset_data.uri;
}

bool PathFinder::checkExist(const std::string& path_or_file)
{
  return (access(path_or_file.c_str(), F_OK) == 0);
}
