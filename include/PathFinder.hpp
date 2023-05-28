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
  PathFinder(Response response_data);
  ~PathFinder();
};

#endif