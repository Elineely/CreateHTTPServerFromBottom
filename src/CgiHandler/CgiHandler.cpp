// #include "CgiHandler.hpp"
#include "../../include/CgiHandler.hpp"

#include <fcntl.h>

#include "Log.hpp"

#define SUCCESS 0
#define RETURN_ERROR -1

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
CgiHandler::CgiHandler() {}

CgiHandler::CgiHandler(Request& requset_data, Response& response_data)
    : m_request_data(requset_data), m_response_data(response_data)
{
}

CgiHandler::~CgiHandler() {}

Response CgiHandler::get_m_response_data() { return (m_response_data); }

void CgiHandler::setCgiEnv(void)
{
  // test
  // m_response_data.cgi_bin_path = "post_python.py";

  // 수정 필요 ----------------
  std::map<std::string, std::string>::iterator it = m_request_data.headers.begin();
  for (; it != m_request_data.headers.end(); ++it)
  {
    std::cout << it->first << ": " << it->second << std::endl;
  }
  std::string defualt_cgi_script(m_response_data.file_name);

  // 값이 없는 경우는 빈 값으로 두면 생략하는 것과 같은 효과

  // m_env_list.push_back("GATEWAY_INTERFACE=CGI/1.1");

  // m_env_list.push_back("SERVER_NAME=" + SERVER_NAME);
  // m_env_list.push_back("SERVER_PORT=" + SERVER_PORT);
  m_env_list.push_back("SERVER_PROTOCOL=HTTP/1.1");
  // m_env_list.push_back("SERVER_SOFTWARE=cute_webserv/1.0");

  // m_env_list.push_back("REMOTE_ADDR=127.0.0.1");
  // 클라이언트의 도메인 네임. 없으면 NULL이거나, REMOTE_ADDR의 값으로 대체
  // m_env_list.push_back("REMOTE_HOST=127.0.0.1");
  m_env_list.push_back("REQUEST_METHOD=" + m_request_data.method);
  // m_env_list.push_back("X_SECRET_HEADER_FOR_TEST=1");

  if (m_response_data.cgi_bin_path == "")
  {
    // m_env_list.push_back("SCRIPT_NAME=" + defualt_cgi_script);
  }
  else
  {
    // m_env_list.push_back("SCRIPT_NAME=" + m_response_data.cgi_bin_path);
    // m_env_list.push_back("SCRIPT_NAME=" + m_response_data.file_name);
  }

  // 원래는 https://hostname:port/script_name/foo/bar.. 이런 식일 때 script_name
  // 이후의 값을 말함 그러나 우린 CGI 스크립트만 실행시킴(실행시킬 스크립트 파일
  // 이외의 파일을 받지 않기 때문에 빈 값) CGI tester에서 혹시 값을 추가로 줄까?
  // 그러면 설정 필요함
  m_env_list.push_back("PATH_INFO=/");
  // PATH_INFO가 NULL이면 얘도 NULL (값이 있으면, 서버 내의 실제 주소를 담아주면
  // 됨 ex. usr/www/html ..)
  // m_env_list.push_back("PATH_TRANSLATED=");

  // 값을 안 줘도 ""로라도 채워줘야 하는 환경변수
  // m_env_list.push_back("QUERY_STRING=");

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

  // m_env_list.push_back("X_FILE_PATH=" + m_response_data.file_path);
  // m_env_list.push_back("X_UPLOAD_PATH=" + m_response_data.uploaded_path);

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

GetCgiHandler::GetCgiHandler() {}

GetCgiHandler::GetCgiHandler(Request& request_data, Response& response_data)
    : CgiHandler(request_data, response_data)
{
}

GetCgiHandler::~GetCgiHandler() {}

// GetCgiHandler::GetCgiHandler(/* ??? */)
// {}

// GetCgiHandler::GetCgiHandler(const GetCgiHandler& obj)
// {}

GetCgiHandler& GetCgiHandler::operator=(GetCgiHandler const& obj)
{
  if (this != &obj)
  {
    m_request_data = obj.m_request_data;
    m_response_data = obj.m_response_data;
    m_env_list = obj.m_env_list;
    m_env_list_parameter = obj.m_env_list_parameter;
    m_to_child_fds[READ] = obj.m_to_child_fds[READ];
    m_to_child_fds[WRITE] = obj.m_to_child_fds[WRITE];
    m_to_parent_fds[READ] = obj.m_to_parent_fds[READ];
    m_to_parent_fds[WRITE] = obj.m_to_parent_fds[WRITE];
    m_pid = obj.m_pid;
  }
  return (*this);
}

// member functions

void GetCgiHandler::pipeAndFork()
{
  if (pipe(m_to_child_fds) == RETURN_ERROR)
  {
    LOG_ERROR("Failed to create m_to_child_fds pipe");
    throw PipeForkException();
  }

  if (pipe(m_to_parent_fds) == RETURN_ERROR)
  {
    LOG_ERROR("Failed to create m_to_parent_fds pipe");
    close(m_to_child_fds[READ]);
    close(m_to_child_fds[WRITE]);
    throw PipeForkException();
  }

  m_pid = fork();
  if (m_pid == RETURN_ERROR)
  {
    LOG_ERROR("Failed to fork");
    close(m_to_child_fds[READ]);
    close(m_to_child_fds[WRITE]);
    close(m_to_parent_fds[READ]);
    close(m_to_parent_fds[WRITE]);
    throw PipeForkException();
  }
}

void GetCgiHandler::executeCgi()
{
  dup2(m_input_file_fd, STDIN_FILENO);
  dup2(m_output_file_fd, STDOUT_FILENO);

  close(m_input_file_fd);
  close(m_output_file_fd);
  setCgiEnv();
  // const char* cgi_bin_path = m_response_data.cgi_bin_path.c_str();
  // std::string cgi_file = m_response_data.root_path + "/" +
  // m_response_data.file_name;
  const char* argv[] = {"./cgi_tester", NULL};
  const char** envp = &m_env_list_parameter[0];
  if (execve("./cgi_tester", const_cast<char* const*>(argv),
             const_cast<char* const*>(envp)) == RETURN_ERROR)
  {
    // LOG_ERROR("Failed to execve function => strerrno: %s", strerror(errno));
    std::cerr << "error: " << strerror(errno) << std::endl;
    std::vector<char> error_message = makeErrorPage();
    write(STDOUT_FILENO, &error_message[0], error_message.size());
    exit(EXIT_FAILURE);
  }
}

void GetCgiHandler::outsourceCgiRequest(void)
{
  try
  {
    int input_file_fd = open("./tmp_file", O_RDWR | O_CREAT, 0644);
    int output_file_fd = open("./output", O_RDWR | O_CREAT, 0644);

    pipeAndFork();

    if (m_pid == CHILD_PROCESS)
    {
      close(m_to_child_fds[WRITE]);
      close(m_to_child_fds[READ]);
      close(m_to_parent_fds[WRITE]);
      close(m_to_parent_fds[READ]);
      m_input_file_fd = input_file_fd;
      m_output_file_fd = output_file_fd;
      executeCgi();
    }
    else
    {
      close(m_to_child_fds[WRITE]);
      close(m_to_child_fds[READ]);
      close(m_to_parent_fds[WRITE]);
      close(m_to_parent_fds[READ]);
      close(input_file_fd);

      m_response_data.read_pipe_fd = output_file_fd;
      m_response_data.cgi_child_pid = m_pid;
    }
  }
  catch (const std::exception& e)
  {
    LOG_ERROR("catch error %s", e.what());
    m_response_data.body = makeErrorPage();
  }
}

/* ******************** */
/* PostCgiHandler class */
/* ******************** */

PostCgiHandler::PostCgiHandler() {}

PostCgiHandler::~PostCgiHandler() {}

PostCgiHandler::PostCgiHandler(Request& request_data, Response& response_data)
    : CgiHandler(request_data, response_data)
{
  // m_request_data = request_data;
  // m_response_data = response_data;
}

PostCgiHandler::PostCgiHandler(const PostCgiHandler& obj) { *this = obj; }

PostCgiHandler& PostCgiHandler::operator=(PostCgiHandler const& obj)
{
  if (this != &obj)
  {
    m_request_data = obj.m_request_data;
    m_response_data = obj.m_response_data;
    m_env_list = obj.m_env_list;
    m_env_list_parameter = obj.m_env_list_parameter;
    m_to_child_fds[READ] = obj.m_to_child_fds[READ];
    m_to_child_fds[WRITE] = obj.m_to_child_fds[WRITE];
    m_to_parent_fds[READ] = obj.m_to_parent_fds[READ];
    m_to_parent_fds[WRITE] = obj.m_to_parent_fds[WRITE];
    m_pid = obj.m_pid;
  }
  return (*this);
}

// member functions

void PostCgiHandler::pipeAndFork()
{
  // if (pipe(m_to_child_fds) == RETURN_ERROR)
  // {
  //   LOG_ERROR("Failed to create m_to_child_fds pipe");
  //   throw PipeForkException();
  // }

  // if (pipe(m_to_parent_fds) == RETURN_ERROR)
  // {
  //   LOG_ERROR("Failed to create m_to_parent_fds pipe");
  //   close(m_to_child_fds[READ]);
  //   close(m_to_child_fds[WRITE]);
  //   throw PipeForkException();
  // }

  m_pid = fork();
  if (m_pid == RETURN_ERROR)
  {
    LOG_ERROR("Failed to fork");
    // close(m_to_child_fds[READ]);
    // close(m_to_child_fds[WRITE]);
    // close(m_to_parent_fds[READ]);
    // close(m_to_parent_fds[WRITE]);
    throw PipeForkException();
  }
}

void PostCgiHandler::executeCgi()
{
  dup2(m_input_file_fd, STDIN_FILENO);
  dup2(m_output_file_fd, STDOUT_FILENO);

  close(m_input_file_fd);
  close(m_output_file_fd);

  setCgiEnv();
  // const char* cgi_bin_path = m_response_data.cgi_bin_path.c_str();
  // std::string cgi_file = m_response_data.root_path + "/" +
  // m_response_data.file_name;
  const char* argv[] = {"./cgi_tester", NULL};
  const char** envp = &m_env_list_parameter[0];
  if (execve("./cgi_tester", const_cast<char* const*>(argv),
             const_cast<char* const*>(envp)) == RETURN_ERROR)
  {
    // LOG_ERROR("Failed to execve function => strerrno: %s", strerror(errno));
    std::vector<char> error_message = makeErrorPage();
    write(STDOUT_FILENO, &error_message[0], error_message.size());
    exit(EXIT_FAILURE);
  }
}

void PostCgiHandler::outsourceCgiRequest(void)
{
  try
  {
    int input_file_fd = open("./tmp_file", O_RDWR | O_CREAT | O_TRUNC, 0644);
    int output_file_fd = open("./output", O_WRONLY | O_CREAT | O_TRUNC, 0644);

    write(input_file_fd, &m_request_data.body[0], m_request_data.body.size());
    close(input_file_fd);
    input_file_fd = open("./tmp_file", O_RDONLY, 0644);
    m_input_file_fd = input_file_fd;
    m_output_file_fd = output_file_fd;
    pipeAndFork();

    if (m_pid == CHILD_PROCESS)
    {
      // close(m_to_child_fds[WRITE]);
      // close(m_to_child_fds[READ]);
      // close(m_to_parent_fds[WRITE]);
      // close(m_to_parent_fds[READ]);
      executeCgi();
    }
    else
    {
      // close(m_to_child_fds[WRITE]);
      // close(m_to_child_fds[READ]);
      // close(m_to_parent_fds[WRITE]);
      // close(m_to_parent_fds[READ]);
      close(input_file_fd);
      close(output_file_fd);

      LOG_DEBUG("output_file_fd: %d", output_file_fd);
      // m_response_data.read_pipe_fd = output_file_fd;
      m_response_data.cgi_child_pid = m_pid;
    }
  }
  catch (const std::exception& e)
  {
    LOG_ERROR("catch error %s", e.what());
    m_response_data.body = makeErrorPage();
  }
}
