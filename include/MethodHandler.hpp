#ifndef METHODHANDLER_HPP
#define METHODHANDLER_HPP

#include "Config.hpp"
#include "Request.hpp"
#include "Response.hpp"

class MethodHandler
{
 protected:
  Request m_request_data;
  // config 전체 클래스가 필요해보이진 않아요.
  // Config m_config_data;
  Response m_response_data;
  t_server m_server_config;
  std::string m_location_name;
  std::string m_body;

  MethodHandler(const MethodHandler& obj);
  MethodHandler& operator=(MethodHandler const& obj);
  virtual void makeBody() = 0;

 public:
  MethodHandler(void);
  MethodHandler(Request& request_data, Response& response_data,
                t_server& server_config);
  virtual ~MethodHandler(void);
};

// PostMethodHandler, DeleteMethodHandler도 만들기
class GetMethodHandler : public MethodHandler
{
 public:
  GetMethodHandler();
  GetMethodHandler(const GetMethodHandler& obj);
  GetMethodHandler(Request& request_data, Response& response_data,
                   t_server& server_config);
  virtual ~GetMethodHandler(void);
  GetMethodHandler& operator=(GetMethodHandler const& obj);

  void makeBody();

 private:
};
class PostMethodHandler : public MethodHandler
{
 public:
  PostMethodHandler();
  PostMethodHandler(const PostMethodHandler& obj);
  PostMethodHandler(Request& request_data, Response& response_data,
                    t_server& server_config);
  virtual ~PostMethodHandler(void);
  PostMethodHandler& operator=(PostMethodHandler const& obj);

  void makeBody();

 private:
};
class DeleteMethodHandler : public MethodHandler
{
 public:
  DeleteMethodHandler();
  DeleteMethodHandler(const DeleteMethodHandler& obj);
  DeleteMethodHandler(Request& request_data, Response& response_data,
                      t_server& server_config);
  virtual ~DeleteMethodHandler(void);
  DeleteMethodHandler& operator=(DeleteMethodHandler const& obj);

  void makeBody();

 private:
};

class PutMethodHandler : public MethodHandler
{
 public:
  PutMethodHandler();
  PutMethodHandler(const PutMethodHandler& obj);
  PutMethodHandler(Request& request_data, Response& response_data,
                   t_server& server_config);
  virtual ~PutMethodHandler(void);
  PutMethodHandler& operator=(PutMethodHandler const& obj);

  void makeBody();

 private:
};

class HeadMethodHandler : public MethodHandler
{
 public:
  HeadMethodHandler();
  HeadMethodHandler(const HeadMethodHandler& obj);
  HeadMethodHandler(Request& request_data, Response& response_data,
                      t_server& server_config);
  virtual ~HeadMethodHandler(void);
  HeadMethodHandler& operator=(HeadMethodHandler const& obj);

  void makeBody();

 private:
};

#endif