#ifndef CGIHANDLER_HPP
#define CGIHANDLER_HPP

#include <unistd.h>
#include <sys/wait.h>
#include <string>
#include <vector>

#include "Request.hpp"
#include "ResponseGenerator.hpp"

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

  void setCgiEnv(void);
  std::vector<char> makeErrorPage(void);

  virtual int pipeAndFork(void) = 0;
  virtual int executeCgi(void) = 0;
  virtual void getDataFromCgi(void) = 0;

 public:
  CgiHandler(void);
  virtual ~CgiHandler(void);

  virtual void outsourceCgiRequest(void) = 0;

  class PipeForkException : public std::exception
  {
   public :
    const char* what() const throw();
  };

  class ExecveException : public std::exception
  {
   public :
    const char* what() const throw();
  };

  class KqueueException : public std::exception
  {
   public :
    const char* what() const throw();
  };

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
  virtual int pipeAndFork(void);
  virtual int executeCgi(void);
  virtual void getDataFromCgi(void);
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
  virtual int pipeAndFork(void);
  virtual int executeCgi(void);
  virtual void getDataFromCgi(void);
};

#endif