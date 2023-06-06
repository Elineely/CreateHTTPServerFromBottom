#ifndef PATHFINDER_HPP
#define PATHFINDER_HPP

#include <dirent.h>
#include <unistd.h>

#include "./ResponseGenerator.hpp"

class PathFinder
{
 private:
  //canonical form
  PathFinder();
  PathFinder(const PathFinder& origin);
  PathFinder& operator=(PathFinder const& origin);
  
  //member_functions
  bool checkExist(const std::string& path_or_file);
  bool is_directory(const std::string& path);
  void setRoot(std::string root, Response& response_data);
  void setIndex(std::string root, std::string index, Response& response_data);
  void setAutoIndex(std::string auto_index, Response& response_data);
  bool setCgi(std::string locationBlock, t_server server_data,
              Response& response_data);
  void setMethod(std::string method, Response& response_data);
  void setRedirection(std::string redirection, Response& response_data);
  void setUpload(std::string upload, Response& response_data);
  void setRootPath(std::string root_path, Response& response_data);
  void setBasic(std::string method, std::string root, std::string index,
                std::string auto_index, std::string upload,
                std::string redirection, std::string root_path,
                Response& response_data);
  // void test_print_location(t_location& c);
  // void test_print_basics(Response& c);

 public:
 //main_constructor
  PathFinder(Request& requset_data, t_server& server_data,
             Response& response_data);
  ~PathFinder();
};

#endif