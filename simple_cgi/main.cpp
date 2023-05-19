#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include <iostream>

#define ERROR -1

#define READ 0
#define WRITE 1

#define CHILD_PROCESS 0

int main(void)
{
  int pipe_fd[2];
  pid_t pid;

  if (pipe(pipe_fd) == ERROR)
  {
    std::cerr << "pipe error" << std::endl;
  }

  pid = fork();
  if (pid == ERROR)
  {
    std::cerr << "fork error" << std::endl;
  }
  else if (pid == CHILD_PROCESS)
  {
    close(pipe_fd[READ]);

    if (dup2(pipe_fd[WRITE], STDOUT_FILENO) == ERROR)
    {
      std::cerr << "redirecting stdout error" << std::endl;
      return (EXIT_FAILURE);
    }

    char* cgi_bin_path = "./php-cgi";
    char* const argv[] = {cgi_bin_path, "index.php", NULL};
    char* const envp[] = {NULL};

    if (execve(cgi_bin_path, argv, envp) == ERROR)
    {
      std::cerr << "executing cgi error" << std::endl;
      return (EXIT_FAILURE);
    }
  }
  else
  {
    close(pipe_fd[WRITE]);

    char buffer[4096];
    std::string output;
    ssize_t bytes_read;
    while (true)
    {
      bytes_read = read(pipe_fd[READ], buffer, sizeof(buffer));
      if (bytes_read <= 0)
      {
        break;
      }
      output.append(buffer, bytes_read);
    }

    int status;

    waitpid(pid, &status, 0);
    if (WIFEXITED(status) && WEXITSTATUS(status) == 0)
    {
      std::cout << output << std::endl;
    }
    else
    {
      std::cerr << "CGI execution failed" << std::endl;
    }
  }

  return (0);
}