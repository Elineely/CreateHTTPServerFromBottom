#ifndef PATHFINDER_HPP
#define PATHFINDER_HPP

#include <dirent.h>
#include <unistd.h>

#include "Log.hpp"
#include "Request.hpp"
#include "Response.hpp"
#include "ResponseGenerator.hpp"

class PathFinder
{
 private:
  // canonical form
  PathFinder();
  PathFinder(const PathFinder& origin);
  PathFinder& operator=(PathFinder const& origin);

  // member_functions
  bool checkExist(const std::string& path_or_file);
  bool is_directory(const std::string& path);
  void setRoot(std::string root, Response& response_data);
  void setIndex(std::string root, std::string file_name, std::string index_name,
                Response& response_data);
  void setAutoIndex(std::string auto_index, Response& response_data);
  bool setCgi(std::string locationBlock, t_server server_data,
              Response& response_data, Request& requset_data);
  void setMethod(std::string method, Response& response_data);
  void setRedirection(std::string redirection, Response& response_data);
  void setUpload(std::string upload, Response& response_data);
  void setRootPath(std::string root_path, Response& response_data);
  void setBasic(std::string method, std::string root, std::string file_name,
                std::string index_name, std::string auto_index,
                std::string upload, std::string redirection,
                std::string root_path, Response& response_data);
  void setMaxSize(Request request_data, std::string max_body_size);
  void checkMaxSize(Request request_data, long max_body_size);

  // functions for abstraction
  bool isRootBlock(std::string locationBlock, t_server& server_data,
                   Response& response_data, Request& request_data);
  bool isCgiBlock(std::string locationBlock, t_server& server_data,
                  Response& response_data, Request& requset_data);
  void oneSlashInUri(t_server& server_data, std::string locationBlock,
                     Response& response_data, Request& request_data);
  void manySlashesInUri(std::string locationBlock, t_server& server_data,
                        Response& response_data, Request& request_data);
  // bool isEndWithDirectory(std::string locationBlock,
                          // t_location current_location, Response& response_data,
                          // Request& request_data);
  bool isEndWithDirectory(std::string entire_path, Request& request_data,
                               t_location current_location,
                               Response& response_data);
  void isEndWithFileName(std::string entire_path, Request& request_data,
                         t_location current_location, Response& response_data);
  bool firstBlockIsNotLocation(t_server& server_data, std::string location_key,
                               t_location current_location,
                               std::string locationBlock,
                               Response& response_data, Request& request_data);
  void firstBlockIsLocation(std::string location_key, t_server& server_data,
                            std::string locationBlock, Request& request_data,
                            Response& response_data);

 public:
  // main_constructor
  PathFinder(Request& requset_data, t_server& server_data,
             Response& response_data);
  ~PathFinder();
};

#endif
