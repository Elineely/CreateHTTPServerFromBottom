#include <unistd.h>
#include <fcntl.h>

#include <iostream>

#define READ 0
#define WRITE 1
#define BUF_SIZE 65535

int main(void)
{
  pid_t pid;
  // int to_child_pipe[2];
  // int to_parent_pipe[2];

  int body_file_fd = open("./tmp_file", O_RDWR, 0644);
  int output_file_fd = open("./output", O_RDWR | O_CREAT, 0644);

  // pipe(to_child_pipe);
  // pipe(to_parent_pipe);

  pid = fork();

  if (pid == 0)
  {
    dup2(body_file_fd, STDIN_FILENO);
    dup2(output_file_fd, STDOUT_FILENO);
    // dup2(to_parent_pipe[WRITE], STDOUT_FILENO);

    close(body_file_fd);
    close(output_file_fd);
    // close(to_child_pipe[READ]);
    // close(to_child_pipe[WRITE]);
    // close(to_parent_pipe[READ]);
    // close(to_parent_pipe[WRITE]);

    char* argv[] = {"./cgi_tester", NULL};
    char* envp[] = {"CONTENT_LENGTH=80000", "SERVER_PROTOCOL=HTTP/1.1", "CONTENT_TYPE=test/file",
                    "REQUEST_METHOD=POST", "PATH_INFO=/", "X_SECRET_HEADER_FOR_TEST=1", NULL};
    if (execve("./cgi_tester", argv, envp) == -1)
    {
      std::cerr << "error" << std::endl;
      exit(EXIT_FAILURE);
    }
  }
  else
  {
    close(body_file_fd);
    // close(to_child_pipe[READ]);
    // close(to_child_pipe[WRITE]);
    // close(to_parent_pipe[WRITE]);
    // write(to_child_pipe[WRITE], "abcd", 100000);
    wait(NULL);

    char buf[BUF_SIZE];
    std::memset(buf, 0, BUF_SIZE);
    while (true)
    {
      ssize_t read_bytes = read(output_file_fd, buf, BUF_SIZE);
      if (read_bytes == 0)
      {
        break;
      }
      std::cout << buf << std::endl;
    }

    close(output_file_fd);
    // unlink("./tmp_file");
  }
}