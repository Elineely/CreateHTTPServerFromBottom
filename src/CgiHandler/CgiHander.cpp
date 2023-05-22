// #include <fstream>
// #include <string>

#include "CgiHandler.hpp"

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

#define ERROR -1

#define READ 0
#define WRITE 1

#define CHILD_PROCESS 0


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

static int pipeAndFork(int** pipe_fd, pid_t* pid)
{
  if (pipe(*pipe_fd) == ERROR)
  {
    // std::cerr << "pipe error" << std::endl;
    return (ERROR);
  }

  *pid = fork();
  if (*pid == ERROR)
  {
    // std::cerr << "fork error" << std::endl;
    return (ERROR);
  }

  return (0);
}

//member functions

void GetCgiHandler::outsourceCgiRequest(void)
{
  int pipe_fd[2];
  pid_t pid;

  if (pipeAndFork(&pipe_fd, &pid) == ERROR)
  {
    // return (error);
  }

// child process
  if (pid == CHILD_PROCESS)
  {
    close(pipe_fd[READ]);

    if (dup2(pipe_fd[WRITE], STDOUT_FILENO) == ERROR)
    {
      // std::cerr << "redirecting stdout error" << std::endl;
      // throw (error);
    }

    char* cgi_bin_path = "./php-cgi";
    char* const argv[] = {cgi_bin_path, "index.php", NULL};
    char* const envp[] = {NULL};

    if (execve(cgi_bin_path, argv, envp) == ERROR)
    {
      // std::cerr << "executing cgi error" << std::endl;
      // throw (error);
    }
  }

// parent process
    close(pipe_fd[WRITE]);

    char buffer[4096]; // 크기,,?
    std::string content;
    ssize_t bytes_read;
    while (true) // 조건문 수정
    {
      bytes_read = read(pipe_fd[READ], buffer, sizeof(buffer));
      if (bytes_read <= 0)
      {
        break ;
      }
      content.append(buffer, bytes_read);

    int status;

    waitpid(pid, &status, 0);
    if (WIFEXITED(status) && WEXITSTATUS(status) == 0)
    {
      // Response 클래스에 넘겨주기
    }
    else
    {
      std::cerr << "CGI execution failed" << std::endl;
      // throw (error);
    }
  }
}

void GetCgiHandler::giveDataToResponse(void)
{}
