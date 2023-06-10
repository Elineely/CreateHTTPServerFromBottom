#include <unistd.h>

#include <iostream>

#define READ 0
#define WRITE 1

int main(void)
{
  pid_t pid;
  int to_child_pipe[2];
  int to_parent_pipe[2];

  pipe(to_child_pipe);
  pipe(to_parent_pipe);

  pid = fork();

  if (pid == 0)
  {
    dup2(to_child_pipe[READ], STDIN_FILENO);
    dup2(to_parent_pipe[WRITE], STDOUT_FILENO);

    close(to_child_pipe[READ]);
    close(to_child_pipe[WRITE]);
    close(to_parent_pipe[READ]);
    close(to_parent_pipe[WRITE]);

    char* argv[] = {"./cgi_tester", NULL};
    char* envp[] = {"CONTENT_LENGTH=4", "SERVER_PROTOCOL=HTTP/1.1",
                    "REQUEST_METHOD=POST", "PATH_INFO=/", NULL};
    if (execve("./cgi_tester", argv, envp) == -1)
    {
      std::cerr << "error" << std::endl;
      exit(EXIT_FAILURE);
    }
  }
  else
  {
    close(to_child_pipe[READ]);
    close(to_parent_pipe[WRITE]);
    write(to_child_pipe[WRITE], "abcd", 4);
    close(to_child_pipe[WRITE]);
    wait(NULL);

    char buf[1024];
    std::memset(buf, 0, 1024);
    ssize_t read_bytes = read(to_parent_pipe[READ], buf, 1024);
    std::cout << buf << std::endl;

    close(to_parent_pipe[READ]);
  }
}