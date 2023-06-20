#include "CgiHandler.hpp"

// canonical form

CgiHandler::CgiHandler(Request& request_data, Response& response_data)
    : m_request_data(request_data), m_response_data(response_data)
{
}

CgiHandler::CgiHandler(const CgiHandler& obj)
    : m_request_data(obj.m_request_data), m_response_data(obj.m_response_data)
{
}

CgiHandler::~CgiHandler() {}

const char* CgiHandler::PipeForkException::what() const throw()
{
  return ("pipe() or fork() system function error");
}

const char* CgiHandler::ExecutionException::what() const throw()
{
  return ("function error");
}

const char* CgiHandler::KqueueException::what() const throw()
{
  return ("kqueue() function error");
}

// member functions

void CgiHandler::pipeAndFork()
{
  if (pipe(m_to_child_fds) == RETURN_ERROR)
  {
    Log::print(ERROR, "Failed to create m_to_child_fds pipe");
    throw PipeForkException();
  }

  if (pipe(m_to_parent_fds) == RETURN_ERROR)
  {
    Log::print(ERROR, "Failed to create m_to_parent_fds pipe");
    close(m_to_child_fds[READ]);
    close(m_to_child_fds[WRITE]);
    throw PipeForkException();
  }

  m_pid = fork();
  if (m_pid == RETURN_ERROR)
  {
    Log::print(ERROR, "Failed to fork");
    close(m_to_child_fds[READ]);
    close(m_to_child_fds[WRITE]);
    close(m_to_parent_fds[READ]);
    close(m_to_parent_fds[WRITE]);
    throw PipeForkException();
  }
}

void CgiHandler::setCgiEnv(void)
{
  m_env_list.push_back("GATEWAY_INTERFACE=CGI/1.1");
  m_env_list.push_back("SERVER_PROTOCOL=HTTP/1.1");
  m_env_list.push_back("SERVER_SOFTWARE=cute_webserv/1.0");

  m_env_list.push_back("REQUEST_METHOD=" + m_request_data.method);

  m_env_list.push_back("PATH_INFO=/");
  m_env_list.push_back("QUERY_STRING=");

  if (m_request_data.body.size() != 0)
  {
    m_env_list.push_back("CONTENT_LENGTH=" +
                         m_request_data.headers["content-length"]);
  }
  if (m_request_data.headers["content-type"] != "")
  {
    m_env_list.push_back("CONTENT_TYPE=" +
                         m_request_data.headers["content-type"]);
  }
  if (m_request_data.headers["x-secret-header-for-test"] != "")
  {
    m_env_list.push_back("X_SECRET_HEADER_FOR_TEST=1");
    m_env_list.push_back("HTTP_X_SECRET_HEADER_FOR_TEST=1");
  }

  m_env_list.push_back("X_UPLOAD_PATH=" + m_response_data.uploaded_path);

  for (unsigned long i = 0; i < m_env_list.size(); ++i)
  {
    m_env_list_parameter.push_back(m_env_list[i].c_str());
  }
  m_env_list_parameter.push_back(NULL);
}

std::vector<char> CgiHandler::makeErrorPage(void)
{
  std::string status_code("Status: 501 Not Implemented\r\n");
  std::string content_type("Content-type: text/html; charset=UTF-8\r\n\r\n");
  std::string body("Failed.\n");
  std::string error_response(status_code + content_type + body);

  std::vector<char> v_error_response;
  for (unsigned long i = 0; i < error_response.size(); ++i)
  {
    v_error_response.push_back(error_response[i]);
  }
  return (v_error_response);
}
