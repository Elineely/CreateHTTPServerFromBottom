#ifndef METHODHANDLER_HPP
#define METHODHANDLER_HPP

#include "Config.hpp"
#include "Request.hpp"
#include "ResponseGenerator.hpp"

#include <sys/types.h>

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
  Request m_request_data;
  Response m_response_data;

  MethodHandler(void);
  MethodHandler(const MethodHandler& obj);
  MethodHandler& operator=(MethodHandler const& obj);

  void fileToBody(std::string target_file);

  // for generate auto_index
  std::string generateDate(const std::time_t& timestamp);
  std::string generateSize(const long long int& fileSize);
  static bool fileInfoCompare(const FileInfo& fileInfo1,
                              const FileInfo& fileInfo2);
  void autoIndexToBody(std::string target_directory);

 public:
  Request get_m_request_data();
  Response get_m_response_data();

  MethodHandler(Request& request_data, Response& response_data);

  virtual void methodRun() = 0;
  virtual ~MethodHandler(void);
};

// PostMethodHandler, DeleteMethodHandler도 만들기
class GetMethodHandler : public MethodHandler
{
 public:
  GetMethodHandler();
  GetMethodHandler(const GetMethodHandler& obj);
  GetMethodHandler(Request& request_data, Response& response_data);
  virtual ~GetMethodHandler(void);
  GetMethodHandler& operator=(GetMethodHandler const& obj);

  void methodRun();
};

class PostMethodHandler : public MethodHandler
{
 public:
  PostMethodHandler();
  PostMethodHandler(const PostMethodHandler& obj);
  PostMethodHandler(Request& request_data, Response& response_data);
  virtual ~PostMethodHandler(void);
  PostMethodHandler& operator=(PostMethodHandler const& obj);

  void methodRun();
};

class DeleteMethodHandler : public MethodHandler
{
 public:
  DeleteMethodHandler();
  DeleteMethodHandler(const DeleteMethodHandler& obj);
  DeleteMethodHandler(Request& request_data, Response& response_data);
  virtual ~DeleteMethodHandler(void);
  DeleteMethodHandler& operator=(DeleteMethodHandler const& obj);

  void methodRun();
};

class PutMethodHandler : public MethodHandler
{
 public:
  PutMethodHandler();
  PutMethodHandler(const PutMethodHandler& obj);
  PutMethodHandler(Request& request_data, Response& response_data);
  virtual ~PutMethodHandler(void);
  PutMethodHandler& operator=(PutMethodHandler const& obj);

  void methodRun();
};

class HeadMethodHandler : public MethodHandler
{
 public:
  HeadMethodHandler();
  HeadMethodHandler(const HeadMethodHandler& obj);
  HeadMethodHandler(Request& request_data, Response& response_data);
  virtual ~HeadMethodHandler(void);
  HeadMethodHandler& operator=(HeadMethodHandler const& obj);

  void methodRun();
};

#endif