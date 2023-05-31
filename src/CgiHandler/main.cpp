#include <unistd.h>
#include <sys/wait.h>
#include <string>
#include <vector>
#include <iostream>

#include "../../include/CgiHandler.hpp"

int main(int argc, char** argv, char** env)
{

  PostCgiHandler post_handler;
  post_handler.setCgiEnv();
  post_handler.outsourceCgiRequest();
  
  for(int i = 0; i < 100; ++i)
  {
   std::cout << post_handler.m_content_vector[i];
  }
  std::cout << std::endl;

//   int to_child_fds[2];
//   int to_parent_fds[2];
//   pid_t pid;

//   pipe(to_parent_fds);
//   pipe(to_child_fds);
//   pid = fork();

// // child process
//   if (pid == CHILD_PROCESS)
//   {
//     close(to_parent_fds[READ]);
//     close(to_child_fds[WRITE]);

//     dup2(to_child_fds[READ], STDIN_FILENO);
//     close(to_child_fds[READ]);

//     dup2(to_parent_fds[WRITE], STDOUT_FILENO);
//     close(to_parent_fds[WRITE]);

//     char* cgi_bin_path = "./test_python.py"; // t_location {ourcgi_pass}
//     char* const argv[] = {cgi_bin_path, "index.php", NULL}; // t_location {ourcgi_index}
//     // char* const envp[] = {"REQUEST_METHOD=GET", NULL};

//     if (execve(cgi_bin_path, argv, env) == ERROR)
//     { std::cout << "execve error" << std::endl; return -1; }
//   }

// // parent process
//   close(to_parent_fds[WRITE]);
//   close(to_child_fds[READ]);

//   std::string request("request content\n");

//   // if (request.content_length == 0 | request.body == NULL)
//   // { /* throw? */ }

//     if (write(to_child_fds[WRITE], /* request.c_str() */"a", /* sizeof(request) */1) == ERROR)
//     { /* throw error; */ }
//     close(to_child_fds[WRITE]);

//     char buffer[4096];
//     std::vector<char> content_vector;
//     ssize_t bytes_read;

//     while (true)
//     {
//       bytes_read = read(to_parent_fds[READ], buffer, sizeof(buffer));
//       if (bytes_read <= 0)
//       {
//         break ;
//       }
//       for (int i = 0; i < bytes_read; ++i)
//       {
//         content_vector.push_back(buffer[i]);
//       }
//     }
//     close(to_parent_fds[READ]);

//     std::cout << "////////////In parent process, print content_vector//////////" << std::endl;
//     for (int i = 0; i < content_vector.size(); ++i)
//       std::cout << content_vector[i];

//     int status;

//     waitpid(pid, &status, 0);
//     // 세 번째 인자 0 : 자식 프로세스가 종료될 때까지 block 상태

//     // std::cout << "status : " << status << std::endl;
//     // std::cout << "WIFEXITED(status) : " << WIFEXITED(status) << std::endl;
//     // std::cout << "WEXITSTATUS(status) : " << WEXITSTATUS(status) << std::endl;

//     if (WIFEXITED(status) && (WEXITSTATUS(status) == 0))
//     {
//       // MethodHandler에 데이터 넘겨주기
//     }
//     else
//     { /* throw (error); */ }

	return (0);
}
