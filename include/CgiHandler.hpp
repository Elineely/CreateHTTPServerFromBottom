#ifndef CGIHANDLER_HPP
#define CGIHANDLER_HPP

#include <sys/wait.h>
#include <unistd.h>

#include <string>
#include <vector>

#include "Request.hpp"
#include "ResponseGenerator.hpp"
#include "Log.hpp"

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
 protected:
  // member variables
  Request m_request_data;
  Response m_response_data;

  std::vector<std::string> m_env_list;
  std::vector<const char*> m_env_list_parameter;

  std::vector<char> m_content_vector;

  int m_to_child_fds[2];
  int m_to_parent_fds[2];
  // int m_input_file_fd;
  // int m_output_file_fd;
  pid_t m_pid;

  // member functions
  void pipeAndFork(void);
  void setCgiEnv(void);
  std::vector<char> makeErrorPage(void);

  virtual void executeCgi(void) = 0;

 public:
  // canonical form
  CgiHandler(void);
  CgiHandler(Request& requset_data, Response& response_data);
  virtual ~CgiHandler(void);

  // getter funtions
  Response get_m_response_data();

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
 private:
  GetCgiHandler(void);

 public:
  GetCgiHandler(Request& request_data, Response& response_data);
  GetCgiHandler(const GetCgiHandler& obj);
  GetCgiHandler& operator=(GetCgiHandler const& obj);
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
 private:
  PostCgiHandler(void);

 public:
  PostCgiHandler(Request& request_data, Response& response_data);
  PostCgiHandler(const PostCgiHandler& obj);
  PostCgiHandler& operator=(PostCgiHandler const& obj);
  virtual ~PostCgiHandler(void);

  virtual void outsourceCgiRequest(void);

 private:
  virtual void executeCgi(void);
};

#endif