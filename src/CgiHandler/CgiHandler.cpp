#include "CgiHandler.hpp"

#include "Log.hpp"

#define SUCCESS 0
#define ERROR -1

#define READ 0
#define WRITE 1

#define CHILD_PROCESS 0

// 제출 전 수정
// #define SERVER_NAME std::string("10.12.9.2")
// #define SERVER_PORT std::string("80")

/* ************************ */
/* CgiHandler virtual class */
/* ************************ */
// no need to make canonical form for virtual class?
// CgiHandler::CgiHandler() {}

CgiHandler::CgiHandler(Request& requset_data, Response& response_data)
    : m_request_data(requset_data), m_response_data(response_data)
{
}

CgiHandler::~CgiHandler() {}

Response CgiHandler::get_m_response_data() { return (m_response_data); }

void CgiHandler::setCgiEnv(void)
{
  // 수정 필요 ----------------
  std::string defualt_cgi_script("post_python.py");

  // 값이 없는 경우는 빈 값으로 표시되면 생략하는 것과 같은 효과

  m_env_list.push_back("GATEWAY_INTERFACE=CGI/1.1");

  // m_env_list.push_back("SERVER_NAME=" + SERVER_NAME);
  // m_env_list.push_back("SERVER_PORT=" + SERVER_PORT);
  m_env_list.push_back("SERVER_PROTOCOL=HTTP/1.1");
  m_env_list.push_back("SERVER_SOFTWARE=cute_webserv/1.0");

  // m_env_list.push_back("REMOTE_ADDR=127.0.0.1");
  // 클라이언트의 도메인 네임. 없으면 NULL이거나, REMOTE_ADDR의 값으로 대체
  // m_env_list.push_back("REMOTE_HOST=127.0.0.1");
  m_env_list.push_back("REQUEST_METHOD=" + m_request_data.method);

  if (m_response_data.cgi_bin_path == "")
  {
    m_env_list.push_back("SCRIPT_NAME=" + defualt_cgi_script);
  }
  else
  {
    m_env_list.push_back("SCRIPT_NAME=" + m_response_data.cgi_bin_path);
    // m_env_list.push_back("SCRIPT_NAME=" + m_response_data.file_name);
  }

  // 원래는 https://hostname:port/script_name/foo/bar.. 이런 식일 때 script_name
  // 이후의 값을 말함 그러나 우린 CGI 스크립트만 실행시킴(실행시킬 스크립트 파일
  // 이외의 파일을 받지 않기 때문에 빈 값) CGI tester에서 혹시 값을 추가로 줄까?
  // 그러면 설정 필요함
  m_env_list.push_back("PATH_INFO=");
  // PATH_INFO가 NULL이면 얘도 NULL (값이 있으면, 서버 내의 실제 주소를 담아주면
  // 됨 ex. usr/www/html ..)
  m_env_list.push_back("PATH_TRANSLATED=");

  // 값을 안 줘도 ""로라도 채워줘야 하는 환경변수
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

  m_env_list.push_back("X_FILE_PATH=" + m_response_data.file_path);
  m_env_list.push_back("X_UPLOAD_PATH=" + m_response_data.uploaded_path);

  for (int i = 0; i < m_env_list.size(); ++i)
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
  for (int i = 0; i < error_response.size(); ++i)
  {
    v_error_response.push_back(error_response[i]);
  }
  return (v_error_response);
}

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
  return ("something wrong with kqueue()");
}

/* ******************* */
/* GetCgiHandler class */
/* ******************* */

// GetCgiHandler::GetCgiHandler() {}

GetCgiHandler::GetCgiHandler(Request& request_data, Response& response_data)
    : CgiHandler(request_data, response_data)
{
}

GetCgiHandler::~GetCgiHandler() {}

// GetCgiHandler::GetCgiHandler(/* ??? */)
// {}

// GetCgiHandler::GetCgiHandler(const GetCgiHandler& obj)
// {}

// GetCgiHandler& GetCgiHandler::operator=(GetCgiHandler const& obj)
// {
//   if (this != &obj)
//   {
//   }
//   return (*this);
// }

// member functions

void GetCgiHandler::pipeAndFork()
{
  if (pipe(m_to_parent_fds) == ERROR)
  {
    Log::error("[GetCgiHandler] Failed to create pipe");
    throw PipeForkException();
  }

  m_pid = fork();
  if (m_pid == ERROR)
  {
    Log::error("[GetCgiHandler] Failed to fork");
    close(m_to_parent_fds[READ]);
    close(m_to_parent_fds[WRITE]);
    throw PipeForkException();
  }
}

void GetCgiHandler::executeCgi()
{
  close(m_to_parent_fds[READ]);

  if (dup2(m_to_parent_fds[WRITE], STDOUT_FILENO) == ERROR)
  {
    Log::error("[GetCgiHandler] Failed to dup2(%d, %d)", m_to_parent_fds,
               STDOUT_FILENO);
    close(m_to_parent_fds[WRITE]);
    throw PipeForkException();
  }
  close(m_to_parent_fds[WRITE]);

  const char* cgi_bin_path = m_response_data.cgi_bin_path.c_str();
  const char* argv[] = {cgi_bin_path, m_response_data.file_name.c_str(), NULL};
  const char** envp = &m_env_list_parameter[0];

  if (execve(cgi_bin_path, const_cast<char* const*>(argv),
             const_cast<char* const*>(envp)) == ERROR)
  {
    Log::error("[GetCgiHandler] Failed to execve function => strerrno: %s",
               strerror(errno));
    throw ExecutionException();
  }
}

void GetCgiHandler::outsourceCgiRequest(void)
{
  setCgiEnv();
  try
  {
    pipeAndFork();
    if (m_pid == CHILD_PROCESS)
    {
      executeCgi();
    }
    else
    {
      close(m_to_parent_fds[WRITE]);
      m_response_data.read_pipe_fd = m_to_parent_fds[READ];
      m_response_data.cgi_child_pid = m_pid;
    }
  }
  catch (const std::exception& e)
  {
    // std::cerr << e.what() << '\n';
    Log::error("[GetCgiHandler] outsourceCgiRequest catch error");
    std::vector<char> error_message = makeErrorPage();
  }
}

/* ******************** */
/* PostCgiHandler class */
/* ******************** */

// PostCgiHandler::PostCgiHandler() {}

PostCgiHandler::~PostCgiHandler() {}

PostCgiHandler::PostCgiHandler(Request& request_data, Response& response_data)
    : CgiHandler(request_data, response_data)
{
  // m_request_data = request_data;
  // m_response_data = response_data;
}

// PostCgiHandler::PostCgiHandler(/* ??? */)
// {}

// PostCgiHandler::PostCgiHandler(const PostCgiHandler& obj)
// {}

// PostCgiHandler& PostCgiHandler::operator=(PostCgiHandler const& obj)
// {
//   if (this != &obj)
//   {
//   }
//   return (*this);
// }

// member functions

void PostCgiHandler::pipeAndFork()
{
  if (pipe(m_to_child_fds) == ERROR)
  {
    Log::error("[PostCgiHandler] Failed to create m_to_child_fds pipe");
    throw PipeForkException();
  }

  if (pipe(m_to_parent_fds) == ERROR)
  {
    Log::error("[PostCgiHandler] Failed to create m_to_parent_fds pipe");
    close(m_to_child_fds[READ]);
    close(m_to_child_fds[WRITE]);
    throw PipeForkException();
  }

  m_pid = fork();
  if (m_pid == ERROR)
  {
    Log::error("[PostCgiHandler] Failed to fork");
    close(m_to_child_fds[READ]);
    close(m_to_child_fds[WRITE]);
    close(m_to_parent_fds[READ]);
    close(m_to_parent_fds[WRITE]);
    throw PipeForkException();
  }
}

void PostCgiHandler::executeCgi()
{
  close(m_to_parent_fds[READ]);
  close(m_to_child_fds[WRITE]);

  // 부모 프로세스로부터 받은 데이터를 cgi에 표준 입력으로 넘겨주는 dup2
  if (dup2(m_to_child_fds[READ], STDIN_FILENO) == ERROR)
  {
    Log::error("[PostCgiHandler] Failed to dup2(%d, %d)", m_to_child_fds[READ],
               STDIN_FILENO);
    close(m_to_child_fds[READ]);
    close(m_to_parent_fds[WRITE]);
    throw PipeForkException();
  }
  close(m_to_child_fds[READ]);

  // cgi의 표준 출력 반환값을 부모 프로세스에 넘겨주는 dup2
  if (dup2(m_to_parent_fds[WRITE], STDOUT_FILENO) == ERROR)
  {
    Log::error("[PostCgiHandler] Failed to dup2(%d, %d)",
               m_to_parent_fds[WRITE], STDOUT_FILENO);
    close(m_to_parent_fds[WRITE]);
    throw PipeForkException();
  }
  close(m_to_parent_fds[WRITE]);

  const char* cgi_bin_path = m_response_data.cgi_bin_path.c_str();
  const char* argv[] = {cgi_bin_path, m_response_data.file_name.c_str(), NULL};
  const char** envp = &m_env_list_parameter[0];

  if (execve(cgi_bin_path, const_cast<char* const*>(argv),
             const_cast<char* const*>(envp)) == ERROR)
  {
    Log::error("[PostCgiHandler] Failed to execve function => strerrno: %s",
               strerror(errno));
    throw ExecutionException();
  }
}

void PostCgiHandler::outsourceCgiRequest(void)
{
  setCgiEnv();
  try
  {
    pipeAndFork();

    if (m_pid == CHILD_PROCESS)
    {
      executeCgi();
    }
    else
    {
      close(m_to_child_fds[READ]);
      close(m_to_child_fds[WRITE]);
      close(m_to_parent_fds[WRITE]);

      m_response_data.read_pipe_fd = m_to_parent_fds[READ];
      m_response_data.cgi_child_pid = m_pid;
    }
  }
  catch (const std::exception& e)
  {
    // std::cerr << e.what() << '\n';
    Log::error("[PostCgiHandler] outsourceCgiRequest catch error");
    std::vector<char> error_message = makeErrorPage();
  }
}
