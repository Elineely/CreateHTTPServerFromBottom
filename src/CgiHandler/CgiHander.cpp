// #include "CgiHandler.hpp"
#include "../../include/CgiHandler.hpp"

#define SUCCESS 0
#define ERROR -1

#define READ 0
#define WRITE 1

#define CHILD_PROCESS 0

#define DEFAULT_CGI_SCRIPT std::string("test_python.py")
// 맥 자리마다 바뀌는데,, server_name 혹은 server의 IP주소 - 근데 CGI가 구분하나?
#define SERVER_NAME std::string("10.12.9.2")
#define SERVER_PORT std::string("80")


/* //////////////////////////////////////////////////////// */
//CgiHandler class
/* //////////////////////////////////////////////////////// */
// no need to make canonical form for virtual class?
CgiHandler::CgiHandler() {}
CgiHandler::~CgiHandler() {}
// CgiHandler::CgiHandler(/* ??? */) {}
// CgiHandler::CgiHandler(const CgiHandler& obj) {}
// CgiHandler& CgiHandler::operator=(CgiHandler const& obj)
// {
//   if (this != &obj)
//   {
//   }
//   return (*this);
// }


void CgiHandler::setCgiEnv()
{
  // -------------------------------------- for test
m_response_data.cgi_bin_path = std::string("post_python.py");
m_request_data.body.push_back('a');
m_request_data.body.push_back('b');
m_request_data.body.push_back('c');
m_request_data.body.push_back('\0');
// ----------------------------------------

  // 값이 없는 경우는 빈 값으로 표시되면 생략하는 것과 같은 효과

  // 우리 뭐 인증 뭐 씁니까? 클라이언트가 요청하는 거 아니면 필요 없긴 함
  // ex) AUTH_TYPE=Basic, AUTH_TYPE=Digest ..
  // m_env_list.push_back("AUTH_TYPE=");
  m_env_list.push_back("GATEWAY_INTERFACE=CGI/1.1");
  // host name이나 network address 주소.
  // hostname이 여러 개일 경우, 요청문의 호스트 헤더 필드를 골라오면 됨
  // m_server_data.server_name 하면, 여러 개 중에 선택해야 하는 문제,, 요청문의 헤더 정보?
  // 맥 자체의 ip주소를 가져오는 방법,,
  // m_env_list.push_back("SERVER_NAME=" + SERVER_NAME);
  // m_env_list.push_back("SERVER_PORT=" + SERVER_PORT);
  m_env_list.push_back("SERVER_PROTOCOL=HTTP/1.1");
  m_env_list.push_back("SERVER_SOFTWARE=cute_webserv/1.0");

  // 클라이언트의 IP주소
  // m_env_list.push_back("REMOTE_ADDR=127.0.0.1");
  // 클라이언트의 도메인 네임. 없으면 NULL이거나, REMOTE_ADDR의 값으로 대체
  // m_env_list.push_back("REMOTE_HOST=127.0.0.1");
  // 말 그대로 클라이언트에서 사용자 인증할 때 필요한 정보. AUTH_TYPE 설정되어 있으면 얘도 필요한 셈(우리 필요 없?)
  m_env_list.push_back("REMOTE_USER=");
  std::string aaa("POSTT");
  m_env_list.push_back("REQUEST_METHOD=" + aaa/* m_request_data.method */);

  if (m_response_data.cgi_bin_path == "")
  {
    m_env_list.push_back("SCRIPT_NAME=" + DEFAULT_CGI_SCRIPT);
  }
  else
  {
    m_env_list.push_back("SCRIPT_NAME=" + m_response_data.cgi_bin_path);
    // m_env_list.push_back("SCRIPT_NAME=" + m_response_data.file_name);
  }

  // 원래는 https://hostname:port/script_name/foo/bar.. 이런 식일 때 script_name 이후의 값을 말함
  // 그러나 우린 CGI 스크립트만 실행시킴(실행시킬 스크립트 파일 이외의 파일을 받지 않기 때문에 빈 값)
  // CGI tester에서 혹시 값을 추가로 줄까? 그러면 설정 필요함
  m_env_list.push_back("PATH_INFO=");
  // PATH_INFO가 NULL이면 얘도 NULL (값이 있으면, 서버 내의 실제 주소를 담아주면 됨 ex. usr/www/html ..)
  m_env_list.push_back("PATH_TRANSLATED=");

  // 값을 안 줘도 ""로라도 채워줘야 하는 환경변수
  m_env_list.push_back("QUERY_STRING=");

  if (m_request_data.body.size() != 0)
  {
    m_env_list.push_back("CONTENT_LENGTH=" + m_request_data.headers["content-length"]);
  }
  // 헤더에 설정되어 있지 않으면 디폴트 값 뭐더라
  m_env_list.push_back("CONTENT_TYPE=" + m_request_data.headers["content-type"]);

  m_env_list.push_back("X_FILE_PATH=" + m_response_data.file_path);
  m_env_list.push_back("X_UPLOAD_PATH=" + m_response_data.uploaded_path);

  m_env_list.push_back("");

  for (int i = 0; i < m_env_list.size(); ++i)
  {
    m_env_list_parameter.push_back(m_env_list[i].c_str());
  }
}

/* //////////////////////////////////////////////////////// */
//GetCgiHandler class
/* //////////////////////////////////////////////////////// */

GetCgiHandler::GetCgiHandler() {}

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

//member functions

int GetCgiHandler::pipeAndFork()
{
  if (pipe(m_to_parent_fds) == ERROR)
  {
    return (ERROR);
  }

  m_pid = fork();
  if (m_pid == ERROR)
  {
    close(m_to_parent_fds[READ]);
    close(m_to_parent_fds[WRITE]);
    return (ERROR);
  }

  return (0);
}

int GetCgiHandler::executeCgi()
{
    close(m_to_parent_fds[READ]);

    if (dup2(m_to_parent_fds[WRITE], STDOUT_FILENO) == ERROR)
    {
      close(m_to_parent_fds[WRITE]);
      return (ERROR);
    }
    close(m_to_parent_fds[WRITE]);

  const char* cgi_bin_path = m_response_data.cgi_bin_path.c_str();
  const char* argv[] = {cgi_bin_path, m_response_data.file_name.c_str(), NULL};
  const char** envp = &m_env_list_parameter[0];

  if (execve(cgi_bin_path, const_cast<char *const *>(argv), const_cast<char *const *>(envp)) == ERROR)
  {
    // throw (error);
  }

  return (SUCCESS);
}

void GetCgiHandler::getDataFromCgi()
{
  close(m_to_parent_fds[WRITE]);

  char buffer[4096]; // 크기
  ssize_t bytes_read;

  while (true) // 조건문 수정?
  {
    bytes_read = read(m_to_parent_fds[READ], buffer, sizeof(buffer));
    if (bytes_read <= 0)
    {
      break ;
    }
    for (int i = 0; i < bytes_read; ++i)
    {
      m_content_vector.push_back(buffer[i]);
    }
  }
  close(m_to_parent_fds[READ]);
}

void GetCgiHandler::outsourceCgiRequest(void)
{
  if (pipeAndFork() == ERROR)
  {
    // throw (error);
  }

  if (m_pid == CHILD_PROCESS)
  {
    if (executeCgi() == ERROR)
    {
      // throw (error);
    }
    return ;
  }

  getDataFromCgi();

  // kqueue() 처리 필요
    int status;

    waitpid(m_pid, &status, 0);
    // 세 번째 인자 0 : 자식 프로세스가 종료될 때까지 block 상태
    if (WIFEXITED(status) && (WEXITSTATUS(status) == 0))
    {
      // MethodHandler에 content 데이터 넘겨주기
    }
    else
    {
      // throw (error);
    }
  }



/* //////////////////////////////////////////////////////// */
//PostCgiHandler class
/* //////////////////////////////////////////////////////// */

PostCgiHandler::PostCgiHandler() {}

PostCgiHandler::~PostCgiHandler() {}

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

//member functions

int PostCgiHandler::pipeAndFork()
{
  if (pipe(m_to_child_fds) == ERROR)
  {
    return (ERROR);
  }

  if (pipe(m_to_parent_fds) == ERROR)
  {
    close(m_to_child_fds[READ]);
    close(m_to_child_fds[WRITE]);
    return (ERROR);
  }

  m_pid = fork();
  if (m_pid == ERROR)
  {
    close(m_to_child_fds[READ]);
    close(m_to_child_fds[WRITE]);
    close(m_to_parent_fds[READ]);
    close(m_to_parent_fds[WRITE]);
    return (ERROR);
  }

  return (SUCCESS);
}

int PostCgiHandler::executeCgi()
{
  close(m_to_parent_fds[READ]);
  close(m_to_child_fds[WRITE]);

// -------------------------------------------test
// char buf[3];
// read(m_to_child_fds[READ], buf, sizeof(buf));
// std::cout << "executeCgi : " << buf << std::endl;

// 부모 프로세스로부터 받은 데이터를 cgi에 표준 입력으로 넘겨주는 dup2
  if (dup2(m_to_child_fds[READ], STDIN_FILENO) == ERROR)
  {
    close(m_to_child_fds[READ]);
    close(m_to_parent_fds[WRITE]);
    // return (error);
  }
  close(m_to_child_fds[READ]);

// cgi의 표준 출력 반환값을 부모 프로세스에 넘겨주는 dup2
  if (dup2(m_to_parent_fds[WRITE], STDOUT_FILENO) == ERROR)
  {
    close(m_to_parent_fds[WRITE]);
    // return (error);
  }
  close(m_to_parent_fds[WRITE]);

  const char* cgi_bin_path = m_response_data.cgi_bin_path.c_str();
  const char* argv[] = {cgi_bin_path, m_response_data.file_name.c_str(), NULL};
  const char** envp = &m_env_list_parameter[0];

  if (execve(cgi_bin_path, const_cast<char *const *>(argv), const_cast<char *const *>(envp)) == ERROR)
  {
    // throw (error);
  }
  return (SUCCESS);
}

void PostCgiHandler::getDataFromCgi()
{
  close(m_to_parent_fds[WRITE]);
  close(m_to_child_fds[READ]);

// ----------------test
// std::cout << "getDataFromCgi : " << std::endl;
// write(1, &m_request_data.body[0], sizeof(char) * m_request_data.body.size());
// std::cout << std::endl;

  if (write(m_to_child_fds[WRITE], &m_request_data.body[0], sizeof(char) * m_request_data.body.size()) == ERROR)
  {
    close(m_to_parent_fds[READ]);
    close(m_to_child_fds[WRITE]);
    // throw error;
  }
  close(m_to_child_fds[WRITE]); //child가 읽는 파이프에 EOF 신호

  char buffer[4096]; // 크기
  ssize_t bytes_read;

  while (true) // 조건문 수정?
  {
    bytes_read = read(m_to_parent_fds[READ], buffer, sizeof(buffer));
    if (bytes_read <= 0)
    {
      break ;
    }
    for (int i = 0; i < bytes_read; ++i)
    {
      m_content_vector.push_back(buffer[i]);
    }
  }
  close(m_to_parent_fds[READ]);
}

void PostCgiHandler::outsourceCgiRequest(void)
{
  if (pipeAndFork() == ERROR)
  {
    // throw (error);
  }

  if (m_pid == CHILD_PROCESS)
  {
    if (executeCgi() == ERROR)
    {
      // throw error;
    }
    return ;
  }

  getDataFromCgi();

  // kqueue()의 영역
    int status;

    waitpid(m_pid, &status, 0);
    // 세 번째 인자 0 : 자식 프로세스가 종료될 때까지 block 상태
    if (WIFEXITED(status) && (WEXITSTATUS(status) == 0))
    {
      // MethodHandler에 데이터 넘겨주기
    }
    else
    {
      // throw (error);
    }
  }
