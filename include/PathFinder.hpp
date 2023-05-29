#ifndef PATHFINDER_HPP
#define PATHFINDER_HPP

#include <unistd.h>

#include "Response.hpp"

class PathFinder
{
 private:
  PathFinder();
  bool checkExist(const std::string& path_or_file);

 public:
  PathFinder(Request requset_data, t_server server_data, Response &response_data);
  ~PathFinder();
};

#endif