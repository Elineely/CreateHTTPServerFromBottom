#include "CgiHandler.hpp"

#define SUCCESS 0
#define ERROR -1

#define READ 0
#define WRITE 1

#define CHILD_PROCESS 0

/* //////////////////////////////////////////////////////// */
//CgiHandler class
/* //////////////////////////////////////////////////////// */
// no need to make canonical form for virtual class?
CgiHandler::CgiHandler() {}
CgiHandler::~CgiHandler() {}
CgiHandler::CgiHandler(/* ??? */) {}
CgiHandler::CgiHandler(const CgiHandler& obj) {}
CgiHandler& CgiHandler::operator=(CgiHandler const& obj)
{
  if (this != &obj)
  {
  }
  return (*this);
}

void CgiHandler::setCgiEnv()
{
  // 값이 없는 경우는 빈 값으로 표시되면 생략하는 것과 같은 효과

  m_env_list.push_back("AUTH_TYPE=");
  
  if (m_request_data.body.size() != 0)
    m_env_list.push_back("CONTENT_LENGTH=" + m_request_data.headers["content-length"]);

  // 헤더에 설정되어 있지 않으면 무슨 값이더라
  m_env_list.push_back("CONTENT_TYPE=" + m_request_data.headers["content-type"]);

  m_env_list.push_back("GATEWAY_INTERFACE=CGI/1.1");

  std::string cgi_bin_path(m_response_data.get_m_cgi_bin_path());
  m_env_list.push_back("PATH_INFO=" + cgi_bin_path);


}

/* //////////////////////////////////////////////////////// */
//GetCgiHandler class
/* //////////////////////////////////////////////////////// */

GetCgiHandler::GetCgiHandler() {}

GetCgiHandler::~GetCgiHandler() {}

GetCgiHandler::GetCgiHandler(/* ??? */)
{}

GetCgiHandler::GetCgiHandler(const GetCgiHandler& obj)
{}

GetCgiHandler& GetCgiHandler::operator=(GetCgiHandler const& obj)
{
  if (this != &obj)
  {
  }
  return (*this);
}

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

    char* cgi_bin_path = "./php-cgi"; // t_location {ourcgi_pass}
    char* const argv[] = {cgi_bin_path, "index.php", NULL}; // t_location {ourcgi_index}
    char* const envp[] = {NULL};

    if (execve(cgi_bin_path, argv, envp) == ERROR)
    {
      return (ERROR);
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

PostCgiHandler::PostCgiHandler(/* ??? */)
{}

PostCgiHandler::PostCgiHandler(const PostCgiHandler& obj)
{}

PostCgiHandler& PostCgiHandler::operator=(PostCgiHandler const& obj)
{
  if (this != &obj)
  {
  }
  return (*this);
}

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

// 클라이언트의 요청을 처리할 CGI 스크립트를 선택해야 함
// multiple CGI를 구현할 경우, 요청에 어떤 CGI가 필요한지 결정해야 함
  char* cgi_bin_path = "./php-cgi"; // t_location {ourcgi_pass}
  char* const argv[] = {cgi_bin_path, "index.php", NULL}; // t_location {ourcgi_index}
  char* const envp[] = {NULL};

  if (execve(cgi_bin_path, argv, envp) == ERROR)
  {
    // throw (error);
  }
}

void PostCgiHandler::getDataFromCgi()
{
  close(m_to_parent_fds[WRITE]);
  close(m_to_child_fds[READ]);

  if (request_data.content_length == 0 | request.body == NULL)
  {
    close(m_to_parent_fds[READ]);
    close(m_to_child_fds[WRITE]);
    // throw?
  }

  if (write(m_to_child_fds[WRITE], request.body, sizeof(request.body)) == ERROR)
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


/* //////////////////////////////////////////////////////// */
//DeleteCgiHandler class
/* //////////////////////////////////////////////////////// */

DeleteCgiHandler::DeleteCgiHandler() {}

DeleteCgiHandler::~DeleteCgiHandler() {}

DeleteCgiHandler::DeleteCgiHandler(/* ??? */)
{}

DeleteCgiHandler::DeleteCgiHandler(const DeleteCgiHandler& obj)
{}

DeleteCgiHandler& DeleteCgiHandler::operator=(DeleteCgiHandler const& obj)
{
  if (this != &obj)
  {
  }
  return (*this);
}

//member functions

void DeleteCgiHandler::outsourceCgiRequest(void)
{
// 메소드 실행 전에 SERVER_PROTOCOL 확인???????????????????

//   if (pipeAndFork() == ERROR)
//   {
//     // return (error);
//   }

//   if (pid == CHILD_PROCESS)
//   {
//      executeCgi();
//   }

//  getDataFromCgi();

// kqueue()
//     int status;

//     waitpid(pid, &status, 0);
//     // 세 번째 인자 0 : 자식 프로세스가 종료될 때까지 block 상태
//     if (WIFEXITED(status) && (WEXITSTATUS(status) == 0))
//     {
//       // 삭제 후에 CONTENT_LENGTH 값 반영!!!!!!!!!!!!!!!!!!
//       // MethodHandler에 데이터 넘겨주기
//     }
//     else
//     {
//       // throw (error);
//     }
  }
