#include "PathFinder.hpp"

PathFinder::PathFinder(/* args */) {}

PathFinder::~PathFinder() {}

bool PathFinder::checkExist(const std::string& path_or_file)
{
  return (access(path_or_file.c_str(), F_OK) == 0);
}
