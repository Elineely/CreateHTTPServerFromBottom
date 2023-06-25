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
#include <map>
#include <set>
#include <vector>

// common I/O header
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <sstream>

// kill function
#include <signal.h>

// Paser header
#include "Config.hpp"
#include "Parser.hpp"
#include "Response.hpp"
#include "ResponseGenerator.hpp"
#include "utils.hpp"

// Server 세팅
#define BUF_SIZE 650000
#define MAX_EVENT_LIST_SIZE 1000
#define DEFAULT_TIMEOUT_SECOND 3600

// 한 소켓에 최대로 기다릴 수 있는 요청의 수
#define BACK_LOG SOMAXCONN

// 실행프로그램 기준으로 상대 경로를 정의한다
#define DEFAULT_SERVER_FILE "./default.conf"
#define SERVER_CONTENT_FILE "./server_content.conf"

enum e_event_type
{
  SERVER = 0,
  CLIENT,
  PIPE,
  PROCESS,
  STATIC_FILE
};

enum e_kqueue_event
{
  SERVER_READ = 0,
  SERVER_WRITE,
  SERVER_ERROR,
  CLIENT_READ,
  CLIENT_WRITE,
  CLIENT_ERROR,
  CGI_PROCESS_TIMEOUT,
  SERVER_EOF,
  CLIENT_EOF,
  PIPE_WRITE,
  PIPE_READ,
  PIPE_EOF,
  STATIC_FILE_READ,
  STATIC_FILE_WRITE,
  STATIC_FILE_EOF,
  NOTHING
};

enum e_pipe
{
  READ,
  WRITE
};

struct t_kqueue
{
  int kq;
  struct kevent event_list[MAX_EVENT_LIST_SIZE];
  std::vector<struct kevent> change_list;
  std::map<int, std::string> socket_clients;
};

struct t_multi_server
{
  int server_sock;
  int server_port;
  struct sockaddr_in serv_addr;
};

struct t_response_write
{
  std::vector<char> message;
  ssize_t length;
  ssize_t offset;

  t_response_write(void) : length(0), offset(0) {}

  t_response_write(std::vector<char> message, ssize_t length)
      : message(message), length(length), offset(0)
  {
  }
};

struct t_event_udata
{
  e_event_type m_type;
  int m_read_pipe_fd;
  int m_write_pipe_fd;
  int m_client_sock;
  int m_server_sock;
  pid_t m_child_pid;
  size_t m_file_write_offset;
  size_t m_total_read_byte;
  std::vector<size_t> m_read_bytes;
  std::vector<char *> m_read_buffer;
  std::vector<char> m_result;
  t_response_write m_response_write;
  config_vector m_servers;
  Request *m_request;
  Response *m_response;
  Parser m_parser;
  struct t_event_udata *m_other_udata;
  struct t_event_udata *m_write_udata;

  t_event_udata(e_event_type type, Request *request = NULL,
                Response *response = NULL)
      : m_type(type),
        m_read_pipe_fd(-1),
        m_write_pipe_fd(-1),
        m_client_sock(-1),
        m_server_sock(-1),
        m_child_pid(-1),
        m_file_write_offset(0),
        m_total_read_byte(0),
        m_request(request),
        m_response(response),
        m_other_udata(NULL),
        m_write_udata(NULL)
  {
  }

  t_event_udata(e_event_type type, config_vector config,
                Request *request = NULL, Response *response = NULL)
      : m_type(type),
        m_read_pipe_fd(-1),
        m_write_pipe_fd(-1),
        m_client_sock(-1),
        m_server_sock(-1),
        m_child_pid(-1),
        m_file_write_offset(0),
        m_total_read_byte(0),
        m_servers(config),
        m_request(request),
        m_response(response),
        m_other_udata(NULL),
        m_write_udata(NULL)
  {
  }

  ~t_event_udata() {}
};

class Server
{
 private:
  std::vector<t_multi_server> servers;
  std::set<int> m_fd_set;
  std::map<int, std::set<t_event_udata *> > m_udata_map;

  t_kqueue m_kqueue;
  Server();

 public:
  Server(Config &server);
  Server(const Server &a);
  ~Server();
  Server &operator=(const Server &a);

  void start(void);
  int getKqueue(void);
  const std::vector<t_multi_server> &get_servers(void);

  void addEventToChangeList(std::vector<struct kevent> &change_list,
                            uintptr_t ident, int16_t filter, uint16_t flags,
                            uint32_t fflags, intptr_t data, void *udata);
  void setSocket(std::vector<t_multi_server> &servers);
  void setServers(const Config &server_conf,
                  std::vector<t_multi_server> &servers);
  void startBind(std::vector<t_multi_server> &servers);
  void startListen(std::vector<t_multi_server> &servers, int back_log);
  int clientReadAccept(struct kevent *current_event);

  void clientWriteEvent(struct kevent *current_event);
  void clientReadEvent(struct kevent *current_event);

  void serverErrorEvent(struct kevent *current_event);

  void fileWriteEvent(struct kevent *current_event);
  void pipeReadEvent(struct kevent *current_event);
  void pipeWriteEvent(struct kevent *current_event);
  void pipeEOFevent(struct kevent *current_event);
  void cgiProcessTimeoutEvent(struct kevent *current_event);

  void disconnectSocket(int socket);
  void addServerSocketEvent(std::vector<t_multi_server> &servers,
                            Config &server_conf);

  // read.cpp
  void serverReadEvent(struct kevent *current_event);
  void readClientSocketBuffer(struct kevent *current_event,
                              t_event_udata *current_udata);
  void addCgiRequestEvent(t_event_udata *current_udata, struct Request &request,
                          struct Response &response);
  void addStaticRequestEvent(struct kevent *current_event,
                             t_event_udata *current_udata,
                             struct Request &request,
                             struct Response &response);
  void staticFileReadEvent(struct kevent *current_event);
  t_event_udata *createUdata(e_event_type event_type,
                             struct kevent *current_event,
                             t_event_udata *current_udata,
                             struct Response &response);

  // write.cpp
  void staticFileWriteEvent(struct kevent *current_event);

  void addUdataMap(int fd, t_event_udata *udata);
  void removeUdata(int fd, t_event_udata *udata);
  void clearUdata(void);
};

#endif
