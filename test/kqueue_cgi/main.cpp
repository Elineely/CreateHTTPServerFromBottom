// thread header
#include <pthread.h>
#include <sys/time.h>
// socket header
#include <arpa/inet.h>
#include <netinet/in.h>
#include <signal.h>
#include <sys/event.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>

#include <iostream>

// std::container header
#include <list>
#include <map>
#include <vector>

// common I/O header
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/event.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
// Server Set value
#define BUF_SIZE 1024
#define MAX_EVENT_LIST_SIZE 8

// 한 소켓에 최대로 기다릴 수 있는 요청의 수
#define BACK_LOG 5

// 실행프로그램 기준으로 상대 경로를 정의한다
#define DEFAULT_SERVER_FILE "./server.conf"
#define SERVER_CONTENT_FILE "./server_content.conf"

#define ERROR -1
#define CHILD_PROCESS 0

#define READ 0
#define WRITE 1

enum e_kqueue_event
{
  SERVER_READ = 0,
  SERVER_WRITE,
  SERVER_ERROR,
  CLIENT_READ,
  CLIENT_WRTIE,
  CLIENT_ERROR,
  PROCESS_END
};

struct t_socket
{
  int server_sock;
  struct sockaddr_in serv_addr;
};

struct t_kqueue
{
  int kq;
  struct kevent event_list[MAX_EVENT_LIST_SIZE];
  std::vector<struct kevent> change_list;
  std::map<int, std::string> socket_clients;
};

void addEventToChangeList(std::vector<struct kevent> &change_list,
                          uintptr_t ident, /* identifier for this event */
                          int16_t filter,  /* filter for event */
                          uint16_t flags,  /* general flags */
                          uint32_t fflags, /* filter-specific flags */
                          intptr_t data,   /* filter-specific data */
                          void *udata)
{
  struct kevent temp_event;

  EV_SET(&temp_event, ident, filter, flags, fflags, data, udata);
  change_list.push_back(temp_event);
}

e_kqueue_event getEventStatus(struct kevent *current_event, int server_sock)
{
  if (current_event->flags & EV_ERROR)
  {
    if (current_event->ident == server_sock)
      return SERVER_ERROR;
    else
      return CLIENT_ERROR;
    // disconnect anything
  }
  // can read buff event
  else if (current_event->filter == EVFILT_READ)
  {
    // to server
    if (current_event->ident == server_sock)
    {
      return SERVER_READ;
    }
    // to client
    else
    {
      return CLIENT_READ;
    }
  }
  return PROCESS_END;
}

void executeChildProcess(t_kqueue &m_kqueue)
{
  pid_t pid;
  int pipe_fd[2];
  int current_events;
  struct kevent current_event;

  if (pipe(pipe_fd) == ERROR)
  {
    std::cerr << "pipe error" << std::endl;
    exit(EXIT_FAILURE);
  }

  pid = fork();
  if (pid == ERROR)
  {
    std::cerr << "fork error" << std::endl;
    exit(EXIT_FAILURE);
  }
  // 자식 프로세스
  if (pid == CHILD_PROCESS)
  {
    close(pipe_fd[READ]);

    if (dup2(pipe_fd[WRITE], STDOUT_FILENO) == ERROR)
    {
      std::cerr << "redirecting stdout error" << std::endl;
      return exit(EXIT_FAILURE);
    }

    char *cgi_bin_path = "./php-cgi"; // TODO: php-cgi 는 joonhan 맥북 기준으로 가져왔음. 나중에 파일 따로 가져올 것
    char *const argv[] = {cgi_bin_path, "index.php", NULL};
    char *const envp[] = {NULL};

    if (execve(cgi_bin_path, argv, envp) == ERROR)
    {
      std::cerr << "executing cgi error" << std::endl;
      exit(EXIT_FAILURE);
    }
  }
  // 부모 프로세스
  else
  {
    close(pipe_fd[WRITE]);

    // Set up the event structure
    addEventToChangeList(m_kqueue.change_list, pid, EVFILT_TIMER,
                         EV_ADD | EV_ONESHOT, NOTE_SECONDS, 10, NULL);
    addEventToChangeList(m_kqueue.change_list, pid, EVFILT_PROC, EV_ADD,
                         NOTE_EXIT, 0, NULL);

    while (1)
    {
      current_events = kevent(m_kqueue.kq, &m_kqueue.change_list[0],
                              m_kqueue.change_list.size(), m_kqueue.event_list,
                              MAX_EVENT_LIST_SIZE, NULL);
      m_kqueue.change_list.clear();
      if (current_events == -1)
      {
        std::cout << "kevent() error: " << strerror(errno) << std::endl;
      }

      for (int i = 0; i < current_events; ++i)
      {
        current_event = m_kqueue.event_list[i];
        std::cout << current_event.ident << std::endl;

        if (current_event.filter == EVFILT_PROC)
        {
          std::cout << "=========✅ PROC ✅=========" << std::endl;

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
          std::cout << "👇 output is 👇" << std::endl;
          std::cout << output << std::endl;
          close(pipe_fd[READ]);
          wait(NULL);

          addEventToChangeList(m_kqueue.change_list, current_event.ident,
                               EVFILT_TIMER, EV_DELETE, 0, 0, NULL);
        }
        else if (current_event.filter == EVFILT_TIMER)
        {
          std::cout << "=========⌛️ TIMER ⌛️=========" << std::endl;
          int result = kill(current_event.ident, SIGTERM);
          std::cout << "kill result: " << result << std::endl;
          wait(NULL);

          addEventToChangeList(m_kqueue.change_list, current_event.ident,
                               EVFILT_PROC, EV_DELETE, 0, 0, NULL);
        }
      }
    }
  }
}

void Server()
{
  t_kqueue m_kqueue;

  m_kqueue.kq = kqueue();
  executeChildProcess(m_kqueue);
}

int main(int argc, char **argv)
{
  Server();
  std::cout << "wellcome Cute webserv!!" << std::endl;
}