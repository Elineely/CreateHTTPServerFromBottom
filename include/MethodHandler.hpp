#ifndef METHODHANDLER_HPP
#define METHODHANDLER_HPP

#include "Config.hpp"
#include "Request.hpp"
#include "ResponseGenerator.hpp"

// generate auto_index
struct FileInfo
{
  std::string is_dir;
  std::string name;
  std::time_t date;
  long long int size;
};

class MethodHandler
{
 protected:
  MethodHandler(const MethodHandler& obj);

  void fileToBody(std::string target_file);

  // for generate auto_index
  std::string generateDate(const std::time_t& timestamp);
  std::string generateSize(const long long int& fileSize);
  static bool fileInfoCompare(const FileInfo& fileInfo1,
                              const FileInfo& fileInfo2);
  void autoIndexToBody(std::string target_directory);

 public:
  Request& m_request_data;
  Response& m_response_data;

  MethodHandler(Request& request_data, Response& response_data);

  virtual void methodRun() = 0;
  virtual ~MethodHandler(void);
};

// PostMethodHandler, DeleteMethodHandler도 만들기
class GetMethodHandler : public MethodHandler
{
 public:
  GetMethodHandler(const GetMethodHandler& obj);
  GetMethodHandler(Request& request_data, Response& response_data);
  virtual ~GetMethodHandler(void);

  void methodRun();
};

class PostMethodHandler : public MethodHandler
{
 public:
  PostMethodHandler(const PostMethodHandler& obj);
  PostMethodHandler(Request& request_data, Response& response_data);
  virtual ~PostMethodHandler(void);

  void methodRun();
};

class DeleteMethodHandler : public MethodHandler
{
 public:
  DeleteMethodHandler(const DeleteMethodHandler& obj);
  DeleteMethodHandler(Request& request_data, Response& response_data);
  virtual ~DeleteMethodHandler(void);

  void methodRun();
};

class PutMethodHandler : public MethodHandler
{
 public:
  PutMethodHandler(const PutMethodHandler& obj);
  PutMethodHandler(Request& request_data, Response& response_data);
  virtual ~PutMethodHandler(void);

  void methodRun();
};

#endif