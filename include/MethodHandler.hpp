#ifndef METHODHANDLER_HPP
#define METHODHANDLER_HPP

#include "Config.hpp"
#include "Request.hpp"
#include "Response.hpp"
#include "ResponseGenerator.hpp"

// struct for generating auto_index page
struct FileInfo
{
  std::string is_dir;
  std::string name;
  std::time_t date;
  size_t size;
};

// base class for MethodHandler classes
class MethodHandler
{
 protected:
  MethodHandler(const MethodHandler& obj);

  // for generating auto_index page
  std::string generateDate(const std::time_t& timestamp);
  std::string generateSize(const ssize_t& fileSize);
  static bool fileInfoCompare(const FileInfo& fileInfo1,
                              const FileInfo& fileInfo2);
  void autoIndexToBody(std::string target_directory);

 public:
  Request& m_request_data;
  Response& m_response_data;

  MethodHandler(Request& request_data, Response& response_data);

  // 이 클래스를 상속 받은 클래스에서 주력으로 사용할 함수
  // 각 method에서 하는 일에 맞게 fd를 open해준 뒤,
  // method의 실질적인 수행은 read.cpp파일을 보면 됨.
  virtual void methodRun() = 0;
  virtual ~MethodHandler(void);
};

// also, it can be used for method "HEAD"
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
