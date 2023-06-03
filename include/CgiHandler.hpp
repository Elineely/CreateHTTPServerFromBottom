#ifndef CGIHANDLER_HPP
#define CGIHANDLER_HPP

#include <unistd.h>
#include <sys/wait.h>
#include <string>
#include <vector>

#include "Request.hpp"
// #include "ResponseGenerator.hpp"
#include <iostream>
struct Response
{
  std::string accepted_method;
  bool redirection_exist;
  std::string rediretion_location;
  bool auto_index;
  std::string file_name;
  bool file_exist;
  std::string file_path;
  bool path_exist;
  std::vector<char> body;
  StatusCode status_code;
  bool cgi_flag;
  std::string cgi_bin_path;
  std::string uploaded_path;
  std::vector<char> response_message;
  int read_pipe_fd;
  int cgi_child_pid;
};


/* ************************ */
/* CgiHandler virtual class */
/* ************************ */

class CgiHandler
{
  protected:
    // member variables
    Request m_request_data;
    Response m_response_data;

    std::vector<std::string> m_env_list;
    std::vector<const char*> m_env_list_parameter;

    int m_to_child_fds[2];
    int m_to_parent_fds[2];
    pid_t m_pid;

    // member functions
    void setCgiEnv(void);
    std::vector<char> makeErrorPage(void);

    virtual void pipeAndFork(void) = 0;
    virtual void executeCgi(void) = 0;
    virtual void getDataFromCgi(void) = 0;

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
      public :
        const char* what() const throw();
    };

    class ExecutionException : public std::exception
    {
      public :
        const char* what() const throw();
    };

    class KqueueException : public std::exception
    {
      public :
        const char* what() const throw();
    };
};


/* ******************* */
/* GetCgiHandler class */
/* ******************* */
class GetCgiHandler : public CgiHandler
{
  // private:
    public :
    GetCgiHandler(void);

  public:
    GetCgiHandler(Request& request_data, Response& response_data);
    GetCgiHandler(const GetCgiHandler& obj);
    GetCgiHandler& operator=(GetCgiHandler const& obj);
    virtual ~GetCgiHandler(void);

    virtual void outsourceCgiRequest(void);

  private:
    virtual void pipeAndFork(void);
    virtual void executeCgi(void);
    virtual void getDataFromCgi(void);

};


/* ******************** */
/* PostCgiHandler class */
/* ******************** */
class PostCgiHandler : public CgiHandler
{
  // private:
    public :
    PostCgiHandler(void);
    
  public:
    PostCgiHandler(Request& request_data, Response& response_data);
    PostCgiHandler(const PostCgiHandler& obj);
    PostCgiHandler& operator=(PostCgiHandler const& obj);
    virtual ~PostCgiHandler(void);

    virtual void outsourceCgiRequest(void);

  private:
    virtual void pipeAndFork(void);
    virtual void executeCgi(void);
    virtual void getDataFromCgi(void);
};

#endif