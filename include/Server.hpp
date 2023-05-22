#ifndef SERVER_HPP
#define SERVER_HPP

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
#include <vector>

// common I/O header
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <sstream>

// Paser header
#include "Config.hpp"
#include "Parser.hpp"

// Server Set value
#define BUF_SIZE 1024
#define MAX_EVENT_LIST_SIZE 8

// 한 소켓에 최대로 기다릴 수 있는 요청의 수
#define BACK_LOG 5

// 실행프로그램 기준으로 상대 경로를 정의한다
#define DEFAULT_SERVER_FILE "./server.conf"
#define SERVER_CONTENT_FILE "./server_content.conf"

enum e_kqueue_event
{
  SERVER_READ = 0,
  SERVER_WRITE,
  SERVER_ERROR,
  CLIENT_READ,
  CLIENT_WRITE,
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
struct t_response_write
{
  t_response_write() {}
  char *message;
  ssize_t send_byte;
};
class Server
{
 private:
  t_socket m_socket;
  t_kqueue m_kqueue;
  Config server;
  Server();

 public:
  Server(Config server);
  Server(const Server &a);
  ~Server();
  Server &operator=(const Server &a);

  int getKqueue();
  void setSocket(Config server_conf);
  void startBind(int server_sock, const struct sockaddr *server_addr);
  void startListen(int server_sock, int back_log);
};

#endif