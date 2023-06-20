#ifndef CGIHANDLER_HPP
#define CGIHANDLER_HPP

#include <sys/wait.h>
#include <unistd.h>

#include <string>
#include <vector>

#include "Log.hpp"
#include "Request.hpp"
#include "Response.hpp"
#include "ResponseGenerator.hpp"
#include "Server.hpp"

#define SUCCESS 0
#define RETURN_ERROR -1

#define READ 0
#define WRITE 1

#define CHILD_PROCESS 0

/* **************** */
/* CgiHandler class */
/* **************** */

class CgiHandler
{
 public:
  Request& m_request_data;
  Response& m_response_data;

 protected:
  // member variables
  std::vector<std::string> m_env_list;
  std::vector<const char*> m_env_list_parameter;

  std::vector<char> m_content_vector;

  int m_to_child_fds[2];
  int m_to_parent_fds[2];
  pid_t m_pid;

  // member functions
  void pipeAndFork(void);
  void setCgiEnv(void);
  std::vector<char> makeErrorPage(void);

  virtual void executeCgi(void) = 0;

 public:
  // canonical form
  CgiHandler(Request& request_data, Response& response_data);
  CgiHandler(const CgiHandler& obj);
  virtual ~CgiHandler(void);

  // member funtions
  virtual void outsourceCgiRequest(void) = 0;

  // exception classes
  class PipeForkException : public std::exception
  {
   public:
    const char* what() const throw();
  };

  class ExecutionException : public std::exception
  {
   public:
    const char* what() const throw();
  };

  class KqueueException : public std::exception
  {
   public:
    const char* what() const throw();
  };
};

/* ******************* */
/* GetCgiHandler class */
/* ******************* */
class GetCgiHandler : public CgiHandler
{
 public:
  GetCgiHandler(Request& request_data, Response& response_data);
  GetCgiHandler(const GetCgiHandler& obj);
  virtual ~GetCgiHandler(void);

  virtual void outsourceCgiRequest(void);

 private:
  virtual void executeCgi(void);
};

/* ******************** */
/* PostCgiHandler class */
/* ******************** */
class PostCgiHandler : public CgiHandler
{
 public:
  PostCgiHandler(Request& request_data, Response& response_data);
  PostCgiHandler(const PostCgiHandler& obj);
  virtual ~PostCgiHandler(void);

  virtual void outsourceCgiRequest(void);

 private:
  virtual void executeCgi(void);
};

#endif
