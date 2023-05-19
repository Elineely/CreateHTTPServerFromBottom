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

Server::Server(Config server_conf)
{
  setSocket(server_conf);
  std::cout << "success Socket" << std::endl;

  startBind(m_socket.server_sock, (const struct sockaddr *)&m_socket.serv_addr);
  std::cout << "success bind" << std::endl;

  startListen(m_socket.server_sock, BACK_LOG);
  std::cout << "success listen" << std::endl;

  m_kqueue.kq = getKqueue();
  std::cout << "success Kqueue" << std::endl;

  int client_sock;
  int client_addr_size;
  int current_events;
  struct kevent *current_event;
  struct sockaddr_in client_addr;
  AddEventToChangeList(m_kqueue.change_list, m_socket.server_sock, EVFILT_READ,
                       EV_ADD | EV_ENABLE, 0, 0, NULL);
  while (1)
  {
    // set
    current_events = kevent(m_kqueue.kq, &m_kqueue.change_list[0],
                            m_kqueue.change_list.size(), m_kqueue.event_list,
                            MAX_EVENT_LIST_SIZE, NULL);
    if (current_events == -1)
    {
      std::cout << "kevent() error" << strerror(errno) << std::endl;
    }
    m_kqueue.change_list.clear();

    for (int i = 0; i < current_events; ++i)
    {
      current_event = &m_kqueue.event_list[i];

      e_kqueue_event event_status;
      event_status = getEventStatus(current_event, m_socket.server_sock);
      switch (event_status)
      {
        case SERVER_READ:
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
          break;
        case SERVER_WRITE:
          break;

        case SERVER_ERROR:
          std::cout << "server socket error" << std::endl;
          break;

        case CLIENT_READ:
          if (m_kqueue.socket_clients.find(current_event->ident) !=
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
          break;

        case CLIENT_WRTIE:
          break;

        case CLIENT_ERROR:
          std::cout << "client socket error" << std::endl;
          break;

        case PROCESS_END:
          break;

        default:
          break;
      }
    }
  }
  shutdown(m_socket.server_sock, SHUT_RDWR);
  close(m_socket.server_sock);
  return;
}

Server::Server() { std::cout << "Server Constructor Call" << std::endl; }

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
