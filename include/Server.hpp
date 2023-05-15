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

#define BUF_SIZE 1024
#define MAX_EVENT_LIST_SIZE 8

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
};

#endif