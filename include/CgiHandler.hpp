#ifndef CGIHANDLER_HPP
#define CGIHANDLER_HPP

#include <unistd.h>
#include <sys/wait.h>
#include <string>
#include <vector>

#include "Config.hpp"
#include "Request.hpp"

// CgiHandler virtual class
class CgiHandler
{
 protected:
  Request m_request_data;
  Response m_response_data;

  std::vector<std::string> m_env_list;
  std::vector<const char*> m_env_list_parameter;

  std::vector<char> m_content_vector;

  int m_to_child_fds[2];
  int m_to_parent_fds[2];
  pid_t m_pid;

 public:
  CgiHandler(void);
  // // CgiHandler(/* ??? */);
  virtual ~CgiHandler(void);

  void setCgiEnv();

  virtual int pipeAndFork() = 0;
  virtual int executeCgi() = 0;
  virtual void getDataFromCgi() = 0;

  virtual void outsourceCgiRequest(void) = 0;

 private:
  CgiHandler(const CgiHandler& obj);
  CgiHandler& operator=(CgiHandler const& obj);
};


// /////////////////////////////////////////////////////////////
// class GetCgiHandler
class GetCgiHandler : public CgiHandler
{
 public:
  GetCgiHandler(void);
  // GetCgiHandler(/* ??? */);
  // GetCgiHandler(const GetCgiHandler& obj);
  // GetCgiHandler& operator=(GetCgiHandler const& obj);
  virtual ~GetCgiHandler(void);

  virtual void outsourceCgiRequest(void);

 private:

  virtual int pipeAndFork();
  virtual int executeCgi();
  virtual void getDataFromCgi();
};

// /////////////////////////////////////////////////////////////
// class PostCgiHandler
class PostCgiHandler : public CgiHandler
{
 public:
  PostCgiHandler(void);
  // // PostCgiHandler(/* ??? */);
  // PostCgiHandler(const PostCgiHandler& obj);
  // PostCgiHandler& operator=(PostCgiHandler const& obj);
  virtual ~PostCgiHandler(void);

  virtual void outsourceCgiRequest(void);

 private:
  virtual int pipeAndFork();
  virtual int executeCgi();
  virtual void getDataFromCgi();
};

#endif