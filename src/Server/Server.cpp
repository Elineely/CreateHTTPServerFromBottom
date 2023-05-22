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
  else if (current_event->filter == EVFILT_WRITE)
  {
    // to server
    if (current_event->ident == server_sock)
    {
      return SERVER_WRITE;
    }
    // to client
    else
    {
      return CLIENT_WRITE;
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
  e_kqueue_event event_status;
  struct kevent *current_event;
  struct sockaddr_in client_addr;
  AddEventToChangeList(m_kqueue.change_list, m_socket.server_sock, EVFILT_READ,
                       EV_ADD | EV_ENABLE, 0, 0, NULL);
  while (1)
  {
    current_events = kevent(m_kqueue.kq, &m_kqueue.change_list[0],
                            m_kqueue.change_list.size(), m_kqueue.event_list,
                            MAX_EVENT_LIST_SIZE, NULL);
    std::cout << "hi" << std::endl;
    if (current_events == -1)
    {
      std::cout << "hi" << std::endl;
      ft_error_exit(1, strerror(errno));
    }
    m_kqueue.change_list.clear();

    for (int i = 0; i < current_events; ++i)
    {
      current_event = &m_kqueue.event_list[i];

      event_status = getEventStatus(current_event, m_socket.server_sock);
      switch (event_status)
      {
        case SERVER_READ:
        {
          client_sock =
              accept(m_socket.server_sock, (struct sockaddr *)&client_addr,
                     reinterpret_cast<socklen_t *>(&client_addr_size));
          if (client_sock == -1)
          {
            ft_error(1, strerror(errno));
            continue;
          }
          std::cout << "새로운 클라이언트가 연결 되었습니다." << std::endl;
          Parser *parser = new Parser();  // TODO: delete 하는 부분 추가하기
          fcntl(client_sock, F_SETFL, O_NONBLOCK);
          AddEventToChangeList(m_kqueue.change_list, client_sock, EVFILT_READ,
                               EV_ADD | EV_ENABLE, 0, 0, parser);
          m_kqueue.socket_clients[client_sock] = "";
        }
        break;

        case SERVER_ERROR:
        {
          std::cout << "server socket error" << std::endl;
        }
        break;

        case CLIENT_READ:
        {
          Parser *parser = static_cast<Parser *>(current_event->udata);
          char buff[BUF_SIZE];
          int recv_size = recv(current_event->ident, buff, sizeof(buff), 0);

          std::cout << buff << std::endl;
          std::cout << "buff end " << std::endl;
          parser->readBuffer(buff);
          if (parser->get_validation_phase() != COMPLETE)
          {
            continue;
          }
          std::cout << "in here" << std::endl;

          //
          // sendClientHttpMessage(current_event->ident);
          //

          // partial write event();
          // t_response_write response;
          // response.message =
          //     "adjkasdnawjkdnasklncsklcnsdklcnsakasdasdqweasdasdeqwdadqweqwdsad"
          //     "qwdasqwlnaskln";
          // response.send_byte = ft_strlen(response.message);
          AddEventToChangeList(m_kqueue.change_list, current_event->ident,
                               EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, NULL);
        }
        break;

        case PROCESS_END:  // cgi
        {
          // pipe 겂 == response body
          // char *message = response.messageGe(cgi_body);
        }
        break;

        case CLIENT_WRITE:
        {
          char protocol[] = "HTTP/1.1 200 OK\r\n";
          char server[] = "Server:Linux Web Server \r\n";
          char date[] = "Date: Wed, 03 May 2023 05:53:31 GM \r\n";
          char cnt_len[] = "Content-length:10\r\n";
          char cnt_type[BUF_SIZE];
          char buff[BUF_SIZE];

          sprintf(cnt_type, "Content-Type: text/plain\r\n\r\n<h1>hi<h1>");
          send(current_event->ident, protocol, strlen(protocol), 0);
          send(current_event->ident, server, strlen(server), 0);
          send(current_event->ident, date, strlen(date), 0);
          send(current_event->ident, cnt_len, strlen(cnt_len), 0);
          send(current_event->ident, cnt_type, strlen(cnt_type), 0);

          close(current_event->ident);
          m_kqueue.socket_clients.erase(current_event->ident);

          // (t_response_write *)current_event->udata;
        }
        break;

        case CLIENT_ERROR:
        {
          std::cout << "client socket error" << std::endl;
        }
        break;

        default:
        {
          std::cout << "status == " << event_status << std::endl;
        }
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
