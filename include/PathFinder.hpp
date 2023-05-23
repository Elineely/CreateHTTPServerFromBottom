#ifndef PATHFINDER_HPP
#define PATHFINDER_HPP

#include "Response.hpp"

class PathFinder
{
 private:
  PathFinder();

 public:
  PathFinder(Response response_data);
  ~PathFinder();
};

#endif