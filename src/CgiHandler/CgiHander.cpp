#include "CgiHandler.hpp"

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

//static functions

static int pipeAndFork(int (*to_parent_fds)[2], pid_t* pid)
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
    std::vector<char> content_vector;
    ssize_t bytes_read;

    while (true) // 조건문 수정?
    {
      bytes_read = read(to_parent_fds[READ], buffer, sizeof(buffer));
      if (bytes_read <= 0)
      {
        break ;
      }
      for (int i = 0; i < bytes_read; ++i)
      {
        content_vector.push_back(buffer[i]);
      }
    }
    close(to_parent_fds[READ]);

    int status;

    waitpid(pid, &status, 0);
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

//static functions

static int pipe2AndFork(int (*to_child_fds)[2], int (*to_parent_fds)[2], pid_t* pid)
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

  if (pipe2AndFork(&to_child_fds, &to_parent_fds, &pid) == ERROR)
  {
    // return (error);
  }

// child process
  if (pid == CHILD_PROCESS)
  {
    close(to_parent_fds[READ]);
    close(to_child_fds[WRITE]);

// 부모 프로세스로부터 받은 데이터를 cgi에 표준 입력으로 넘겨주는 dup2
    if (dup2(to_child_fds[READ], STDIN_FILENO) == ERROR)
    {
      // throw (error);
    }
    close(to_child_fds[READ]);

// cgi의 표준 출력 반환값을 부모 프로세스에 넘겨주는 dup2
    if (dup2(to_parent_fds[WRITE], STDOUT_FILENO) == ERROR)
    {
      // throw (error);
    }
    close(to_parent_fds[WRITE]);

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

// parent process
    close(to_parent_fds[WRITE]);
    close(to_child_fds[READ]);

  if (request.content_length == 0 | request.body == NULL)
  {
    // throw?
  }

    if (write(to_child_fds[WRITE], request.body, sizeof(request.body)) == ERROR)
    {
      // throw error;
    }
    close(to_child_fds[WRITE]); //child가 읽는 파이프에 EOF 신호

    char buffer[4096]; // 크기
    std::vector<char> content_vector;
    ssize_t bytes_read;

    while (true) // 조건문 수정?
    {
      bytes_read = read(to_parent_fds[READ], buffer, sizeof(buffer));
      if (bytes_read <= 0)
      {
        break ;
      }
      for (int i = 0; i < bytes_read; ++i)
      {
        content_vector.push_back(buffer[i]);
      }
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

//static functions

static int pipe2AndFork(int (*to_child_fds)[2], int (*to_parent_fds)[2], pid_t* pid)
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

void DeleteCgiHandler::outsourceCgiRequest(void)
{
// 메소드 실행 전에 SERVER_PROTOCOL 확인???????????????????

//   int to_child_fds[2];
//   int to_parent_fds[2];
//   pid_t pid;

//   if (pipe2AndFork(&to_child_fds, &to_parent_fds, &pid) == ERROR)
//   {
//     // return (error);
//   }

// // child process
//   if (pid == CHILD_PROCESS)
//   {
//     close(to_parent_fds[READ]);
//     close(to_child_fds[WRITE]);

// // 부모 프로세스로부터 받은 데이터를 cgi에 표준 입력으로 넘겨주는 dup2
//     if (dup2(to_child_fds[READ], STDIN_FILENO) == ERROR)
//     {
//       // throw (error);
//     }
//     close(to_child_fds[READ]);

//     if (dup2(to_parent_fds[WRITE], STDOUT_FILENO) == ERROR)
//     {
//       // throw (error);
//     }
//     close(to_parent_fds[WRITE]);

//     char* cgi_bin_path = "./php-cgi"; // t_location {ourcgi_pass}
//     char* const argv[] = {cgi_bin_path, "index.php", NULL}; // t_location {ourcgi_index}
//     char* const envp[] = {NULL};

//     if (execve(cgi_bin_path, argv, envp) == ERROR)
//     {
//       // throw (error);
//     }
//   }

// // parent process
//     close(to_parent_fds[WRITE]);
//     close(to_child_fds[READ]);

//     // request의 body만 넘기는 거면, 세 번째 인자는 CONTENT_LENGTH 쓰면 됨
//     if (write(to_child_fds[WRITE], /* request_data */ , sizeof()) == ERROR)
//     {
//       // throw error;
//     }
//     close(to_child_fds[WRITE]); //child가 읽는 파이프에 EOF 신호

//     char buffer[4096]; // 크기
//     std::string content;
//     ssize_t bytes_read;

//     while (true) // 조건문 수정?
//     {
//       bytes_read = read(to_parent_fds[READ], buffer, sizeof(buffer));
//       if (bytes_read <= 0)
//       {
//         break ;
//       }
//       content.append(buffer, bytes_read);
//     }
//     close(to_parent_fds[READ]);

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
