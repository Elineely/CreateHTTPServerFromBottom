// thread header
#include <pthread.h>
#include <sys/time.h>
// socket header
#include <arpa/inet.h>
#include <netinet/in.h>
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
#include <unistd.h>

#include <sstream>

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

void AddEventToChangeList(std::vector<struct kevent> &change_list,
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

void Server()
{
  t_kqueue m_kqueue;
  int current_events;
  struct kevent *current_event;
  int pipe_fd[2];

  m_kqueue.kq = kqueue();
  int pid = fork();
  // 자식 process
  if (pid == CHILD_PROCESS)
  {
    std::cout << "Child sleeps" << std::endl;
    sleep(1);
    std::cout << "Child wakes up" << std::endl;
    exit(0);
  }
  AddEventToChangeList(m_kqueue.change_list, pid, EVFILT_PROC, EV_ADD,
                       NOTE_EXIT, 0, NULL);
  current_events = kevent(m_kqueue.kq, &m_kqueue.change_list[0], 1,
                          m_kqueue.event_list, 1, NULL);
  if (current_events == -1)
  {
    std::cout << "kevent() error" << strerror(errno) << std::endl;
  }
  for (int i = 0; i < current_events; ++i)
  {
    current_event = &m_kqueue.event_list[i];

    if (current_event->filter == EVFILT_PROC)
    {
      if (current_event->fflags == NOTE_EXIT)
      {
        std::cout << "find exit child process" << std::endl;
      }
    }
  }
  return;
}

int main(int argc, char **argv)
{
  Server();
  std::cout << "wellcome Cute webserv!!" << std::endl;
}