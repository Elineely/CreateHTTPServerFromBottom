// #include <fstream>
// #include <string>

#include "CgiHandler.hpp"

#define ERROR -1

#define READ 0
#define WRITE 1

#define CHILD_PROCESS 0

//CgiHandler class
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


//GetCgiHandler class

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

//static functions

static int pipeAndFork(int** to_parent_fds, pid_t* pid)
{
  if (pipe(*to_parent_fds) == ERROR)
  {
    return (ERROR);
  }

  *pid = fork();
  if (*pid == ERROR)
  {
    return (ERROR);
  }

  return (0);
}

//member functions

void GetCgiHandler::outsourceCgiRequest(void)
{
  int to_parent_fds[2];
  pid_t pid;

  if (pipeAndFork(&to_parent_fds, &pid) == ERROR)
  {
    // return (error);
  }

// child process
  if (pid == CHILD_PROCESS)
  {
    close(to_parent_fds[READ]);

    if (dup2(to_parent_fds[WRITE], STDOUT_FILENO) == ERROR)
    {
      // throw (error);
    }
    close(to_parent_fds[WRITE]);

    char* cgi_bin_path = "./php-cgi"; // t_location {ourcgi_pass}
    char* const argv[] = {cgi_bin_path, "index.php", NULL}; // t_location {ourcgi_index}
    char* const envp[] = {NULL};

    if (execve(cgi_bin_path, argv, envp) == ERROR)
    {
      // throw (error);
    }
  }

// parent process
    close(to_parent_fds[WRITE]);

    char buffer[4096]; // 크기,,?
    std::string content;
    ssize_t bytes_read;

    while (true) // 조건문 수정?
    {
      bytes_read = read(to_parent_fds[READ], buffer, sizeof(buffer));
      if (bytes_read <= 0)
      {
        break ;
      }
      content.append(buffer, bytes_read);
    }
    close(to_parent_fds[READ]);

// 정상 출력 or 위치 설정 잘못된 경우 or 권한 or 헤더 이상의 경우 -> 4개 분기
// 헤더와 body 분리(\n\n)? 통째로?

    int status;

    waitpid(pid, &status, 0);
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

void GetCgiHandler::giveDataToResponse(void)
{}


//PostCgiHandler class

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

//static functions

static int pipe2AndFork(int** to_child_fds, int** to_parent_fds, pid_t* pid)
{
  if (pipe(*to_child_fds) == ERROR)
  {
    return (ERROR);
  }

  if (pipe(*to_parent_fds) == ERROR)
  {
    return (ERROR);
  }

  *pid = fork();
  if (*pid == ERROR)
  {
    return (ERROR);
  }

  return (0);
}

//member functions

void PostCgiHandler::outsourceCgiRequest(void)
{
  int to_child_fds[2];
  int to_parent_fds[2];
  pid_t pid;

  if (pipeAndFork(&to_child_fds, &to_parent_fds, &pid) == ERROR)
  {
    // return (error);
  }

// child process
  if (pid == CHILD_PROCESS)
  {
    close(to_parent_fds[READ]);
    close(to_child_fds[WRITE]);

    // char buffer[4096]; // 크기
    // std::string request_data;
    // ssize_t bytes_read;

    // while (true) // 조건문 수정?
    // {
    //   bytes_read = read(to_child_fds[READ], buffer, sizeof(buffer));
    //   if (bytes_read <= 0)
    //   {
    //     break ;
    //   }
    //   request_data.append(buffer, bytes_read);
    // }
    // close(to_child_fds[READ]);

    if (dup2(to_child_fds[READ], STDIN_FILENO) == ERROR)
    {
      // throw (error);
    }
    close(to_child_fds[READ]);

    if (dup2(to_parent_fds[WRITE], STDOUT_FILENO) == ERROR)
    {
      // throw (error);
    }
    close(to_parent_fds[WRITE]);

    char* cgi_bin_path = "./php-cgi"; // t_location {ourcgi_pass}
    char* const argv[] = {cgi_bin_path, "index.php", NULL}; // t_location {ourcgi_index}
    char* const envp[] = {NULL};
//  request_data를 표준입력 파라미터로 넘겨주려면?
// 아니면 requst_data를 dup2(to_child_fds[READ], STDIN_FILENO)해서 바로 받기?

    if (execve(cgi_bin_path, argv, envp) == ERROR)
    {
      // throw (error);
    }
  }

// parent process
    close(to_parent_fds[WRITE]);
    close(to_child_fds[READ]);

    if (write(to_child_fds[WRITE], /* request_data */ , sizeof(data)) == ERROR)
    // request의 body만 넘기는 거면, 세 번째 인자는 CONTENT_LENGTH 쓰면 됨
    // 데이터 부분에 요청 정보가 들어오는 POST (앞에서 어디까지 분류되나)
    {
      // throw error;
    }
    close(to_child_fds[WRITE]); //child가 읽는 파이프에 EOF 신호

    char buffer[4096]; // 크기
    std::string content;
    ssize_t bytes_read;

    while (true) // 조건문 수정?
    {
      bytes_read = read(to_parent_fds[READ], buffer, sizeof(buffer));
      if (bytes_read <= 0)
      {
        break ;
      }
      content.append(buffer, bytes_read);
    }
    close(to_parent_fds[READ]);

    int status;

    waitpid(pid, &status, 0);
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

void PostCgiHandler::giveDataToResponse(void)
{}

//DeleteCgiHandler (일단 POST와 비슷?)