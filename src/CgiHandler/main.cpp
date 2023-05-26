#include <unistd.h>
#include <sys/wait.h>
#include <string>
#include <vector>

#define ERROR -1

#define READ 0
#define WRITE 1

#define CHILD_PROCESS 0

int main()
{

  int to_child_fds[2];
  int to_parent_fds[2];
  pid_t pid;

  if (pipe(to_parent_fds) == ERROR)
  { /* return (ERROR); */ }

  if (pipe(to_child_fds) == ERROR)
  { /* return (ERROR); */ }

  pid = fork();
  if (pid == ERROR)
  { /* return (ERROR); */ }

// child process
  if (pid == CHILD_PROCESS)
  {
    close(to_parent_fds[READ]);
    close(to_child_fds[WRITE]);

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

    if (execve(cgi_bin_path, argv, envp) == ERROR)
    {
      // throw (error);
    }
  }

// parent process
  close(to_parent_fds[WRITE]);
  close(to_child_fds[READ]);

  std::string request("request content\n");

  // if (request.content_length == 0 | request.body == NULL)
  // { /* throw? */ }

    if (write(to_child_fds[WRITE], /* request.c_str() */"a", /* sizeof(request) */1) == ERROR)
    { /* throw error; */ }
    close(to_child_fds[WRITE]);

    char buffer[4096];
    std::vector<char> content_vector;
    ssize_t bytes_read;

    while (true)
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

	return (0);
}

//member functions

