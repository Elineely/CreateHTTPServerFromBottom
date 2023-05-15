#include "Server.hpp"

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

void sendClientHttpMessage(int clientNum)
{
  char protocol[] = "HTTP/1.1 200 OK\r\n";
  char server[] = "Server:Linux Web Server \r\n";
  char date[] = "Date: Wed, 03 May 2023 05:53:31 GM \r\n";
  char cnt_len[] = "Content-length:10\r\n";
  char cnt_type[BUF_SIZE];
  char buff[BUF_SIZE];

  FILE *send_file;

  // here is size of
  sprintf(cnt_type, "Content-Type: text/plain\r\n\r\n<h1>hi<h1>");
  send(clientNum, protocol, strlen(protocol), 0);
  send(clientNum, server, strlen(server), 0);
  send(clientNum, date, strlen(date), 0);
  send(clientNum, cnt_len, strlen(cnt_len), 0);
  send(clientNum, cnt_type, strlen(cnt_type), 0);
}

Server::Server() { std::cout << "Server Constructor Call" << std::endl; }

Server::Server(Config server_conf)
{
  std::cout << "Server Constructor Call" << std::endl;
  m_socket.server_sock = socket(PF_INET, SOCK_STREAM, 0);
  if (m_socket.server_sock == -1)
  {
    strerror(errno);
    return;
  }
  std::cout << "success socket" << std::endl;
  m_socket.serv_addr.sin_family = AF_INET;
  m_socket.serv_addr.sin_port = htons(server_conf.getServerPort());
  m_socket.serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);

  if (bind(m_socket.server_sock, (const struct sockaddr *)&m_socket.serv_addr,
           sizeof(m_socket.serv_addr)) == -1)
  {
    std::cout << "bind error" << std::endl;
    return;
  }
  std::cout << "success bind" << std::endl;
  if (listen(m_socket.server_sock, 5) == -1)
  {
    std::cout << "listen error" << std::endl;
    return;
  }
  std::cout << "success listen" << std::endl;

  m_kqueue.kq = kqueue();
  if (m_kqueue.kq == -1)
  {
    std::cout << "kqueue() error :" << strerror(errno) << std::endl;
    return;
  }

  int client_sock;
  int client_addr_size;
  struct sockaddr_in client_addr;
  int current_events;
  struct kevent *current_event;
  AddEventToChangeList(m_kqueue.change_list, m_socket.server_sock, EVFILT_READ,
                       EV_ADD | EV_ENABLE, 0, 0, NULL);
  while (1)
  {
    current_events = kevent(m_kqueue.kq, &m_kqueue.change_list[0],
                            m_kqueue.change_list.size(), m_kqueue.event_list,
                            MAX_EVENT_LIST_SIZE, NULL);
    if (current_events == -1)
    {
      std::cout << "kevent() error" << std::endl;
    }
    m_kqueue.change_list.clear();
    for (int i = 0; i < current_events; ++i)
    {
      current_event = &m_kqueue.event_list[i];

      if (current_event->flags & EV_ERROR)
      {
        if (current_event->ident == m_socket.server_sock)
          std::cout << "server socket error" << std::endl;
        else
          std::cout << "client socket error" << std::endl;
        // disconnect anything
      }
      // can read buff event
      else if (current_event->filter == EVFILT_READ)
      {
        // to server
        if (current_event->ident == m_socket.server_sock)
        {
          client_sock =
              accept(m_socket.server_sock, (struct sockaddr *)&client_addr,
                     reinterpret_cast<socklen_t *>(&client_addr_size));
          if (client_sock == -1)
          {
            strerror(errno);
            return;
          }
          std::cout << "새로운 클라이언트가 연결 되었습니다." << std::endl;
          fcntl(client_sock, F_SETFL, O_NONBLOCK);
          AddEventToChangeList(m_kqueue.change_list, client_sock, EVFILT_READ,
                               EV_ADD | EV_ENABLE, 0, 0, NULL);
          m_kqueue.socket_clients[client_sock] = "";
        }
        // to client
        else if (m_kqueue.socket_clients.find(current_event->ident) !=
                 m_kqueue.socket_clients.end())
        {
          char buff[1024];
          int recv_size = recv(current_event->ident, buff, sizeof(buff), 0);
          //
          //  parse section
          //
          Parser paser;
          //
          //  send to HTTP Message section ;
          //
          //
          //  return http message section
          //
          if (recv_size <= 0)
          {
            if (recv_size < 0) std::cout << "client read error" << std::endl;
            // disconnect_ anything
          }
          std::cout << "client message to server : " << buff << std::endl;
          sendClientHttpMessage(current_event->ident);
        }
      }
      // can write buff event
      // else if (EVFILT_WRITE)
      // {

      // }
    }
  }
  shutdown(m_socket.server_sock, SHUT_RDWR);
  close(m_socket.server_sock);
  return;
}

Server::Server(const Server &other)
{
  std::cout << "Server Constructor Call" << std::endl;
  *this = other;
}

Server::~Server() { std::cout << "Server Destructor Call" << std::endl; }

Server &Server::operator=(const Server &other)
{
  if (this == &other) return *this;
  std::cout << "Server Assignment Operator Call" << std::endl;
  return *this;
}
