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

    int status;

    waitpid(pid, &status, 0);
    // 세 번째 인자 0 : 자식 프로세스가 종료될 때까지 block 상태
    if (WIFEXITED(status) && (WEXITSTATUS(status) == 0))
    {
      // Response 클래스에 content 넘겨주기
      // giveDataToResponse();
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

    char buffer[4096]; // 크기
    std::string request_data;
    ssize_t bytes_read;

// 순서대로 주고 받는지 테스트 -> close() 잘하면 순서대로 주고 받음
    while (true) // 조건문 수정?
    {
      bytes_read = read(to_child_fds[READ], buffer, sizeof(buffer));
      if (bytes_read <= 0)
      {
        break ;
      }
      request_data.append(buffer, bytes_read);
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
//  request_data;

// GET -> 바이너리 프로그램에 index.php와 envp 넣으면 html로 만들어줌
// POST, DELETE -> ???
    if (execve(cgi_bin_path, argv, envp) == ERROR)
    {
      // throw (error);
    }
  }

// parent process
    close(to_parent_fds[WRITE]);
    close(to_child_fds[READ]);

    if (write(to_child_fds[WRITE], /* request.data */ , sizeof(data)) == ERROR)
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
      // Response 클래스에 content 넘겨주기
      // giveDataToResponse();
    }
    else
    {
      // throw (error);
    }
  }

void PostCgiHandler::giveDataToResponse(void)
{}
