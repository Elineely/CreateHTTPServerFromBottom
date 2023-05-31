#include "Server.hpp"

// httpProcessor가 완성되기 전 테스트용
#include "beforeHttpProcessor.hpp"

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

void disconnect_socket(int socket) { close(socket); }

int is_server_sock(std::vector<t_multi_server> servers, int sock)
{
  for (int i = 0; i < servers.size(); ++i)
  {
    if (servers[i].server_sock == sock) return 1;
  }
  return 0;
}

e_kqueue_event getEventStatus(struct kevent *current_event,
                              std::vector<t_multi_server> &servers)
{
  if (current_event->flags == EV_ERROR)
  {
    if (is_server_sock(servers, current_event->ident))
      return SERVER_ERROR;
    else
      return CLIENT_ERROR;
  }
  if (current_event->flags == EV_EOF)
  {
    if (is_server_sock(servers, current_event->ident))
      return SERVER_EOF;
    else
      return CLIENT_EOF;
  }
  if (current_event->filter == EVFILT_READ)
  {
    if (is_server_sock(servers, current_event->ident))
      return SERVER_READ;
    else
      return CLIENT_READ;
  }
  if (current_event->filter == EVFILT_WRITE)
  {
    if (is_server_sock(servers, current_event->ident))
      return SERVER_WRITE;
    else
      return CLIENT_WRITE;
  }
  if (current_event->filter == EVFILT_PROC)
  {
    return CGI_PROCESS_END;
  }
  if (current_event->filter == EVFILT_TIMER)
  {
    return CGI_PROCESS_TIMEOUT;
  }
  return NOTHING;
}

Server::Server(Config server_conf)
{
  setServers(server_conf, servers);
  std::cout << "success Server Set" << std::endl;

  setSocket(server_conf, servers);
  std::cout << "success Socket" << std::endl;

  startBind(servers);
  std::cout << "success bind" << std::endl;

  startListen(servers, BACK_LOG);
  std::cout << "success listen" << std::endl;

  m_kqueue.kq = getKqueue();
  std::cout << "success Kqueue" << std::endl;

  int client_sock;
  int client_addr_size;
  int current_events;
  e_kqueue_event event_status;
  struct kevent *current_event;
  struct sockaddr_in client_addr;
  for (int i = 0; i < servers.size(); ++i)
  {
    AddEventToChangeList(m_kqueue.change_list, servers[i].server_sock,
                         EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0,
                         &servers[i].config);
  }
  int kevent_count = 0;
  while (1)
  {
    current_events = kevent(m_kqueue.kq, &m_kqueue.change_list[0],
                            m_kqueue.change_list.size(), m_kqueue.event_list,
                            MAX_EVENT_LIST_SIZE, NULL);
    if (current_events == -1)
    {
      ft_error_exit(1, strerror(errno));
    }
    m_kqueue.change_list.clear();

    for (int i = 0; i < current_events; ++i)
    {
      current_event = &m_kqueue.event_list[i];

      event_status = getEventStatus(current_event, servers);
      switch (event_status)
      {
        case SERVER_READ:
        {
          std::cout << "--- in SERVER_READ" << std::endl;

          client_sock =
              accept(current_event->ident, (struct sockaddr *)&client_addr,
                     reinterpret_cast<socklen_t *>(&client_addr_size));
          if (client_sock == -1)
          {
            ft_error(1, strerror(errno));
            continue;
          }
          std::cout << "새로운 클라이언트가 연결 되었습니다." << std::endl;

          std::string max_body_size =
              static_cast<t_server *>(current_event->udata)->max_body_size[0];
          // TODO: delete 하는 부분 추가하기
          Parser *parser = new Parser(max_body_size);
          fcntl(client_sock, F_SETFL, O_NONBLOCK);
          AddEventToChangeList(m_kqueue.change_list, client_sock, EVFILT_READ,
                               EV_ADD | EV_ENABLE, 0, 0, parser);
          m_kqueue.socket_clients[client_sock] = "";
        }
        break;

        case SERVER_ERROR:
        {
          std::cout << "server socket error" << std::endl;
          disconnect_socket(current_event->ident);
        }
        break;

        case CLIENT_READ:
        {
          Parser *parser = static_cast<Parser *>(current_event->udata);
          char buff[BUF_SIZE];
          std::memset(buff, 0, BUF_SIZE);
          int recv_size = recv(current_event->ident, buff, sizeof(buff), 0);
          parser->readBuffer(buff);
          if (parser->get_validation_phase() != COMPLETE)
          {
            continue;
          }

          // getttpMessage
          std::vector<char> message = getHttpMessage();
          // if (response.cgi == true)
          // {
          //  coutinue;
          // }
          // else
          // {
          t_response_write *response =
              new t_response_write(&message[0], message.size());
          AddEventToChangeList(m_kqueue.change_list, current_event->ident,
                               EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0,
                               response);
          // }
          // }
        }
        break;

        case CGI_PROCESS_END:  // cgi
        {
          std::cout << "CGI_PROCESS_END" << std::endl;

          wait(NULL);
          // pipe close
          AddEventToChangeList(m_kqueue.change_list, current_event->ident,
                               EVFILT_TIMER, EV_DELETE, 0, 0, NULL);

          std::vector<char> message = getHttpMessage();
          // 인자로 response 전달

          t_response_write *response =
              new t_response_write(&message[0], message.size());
          AddEventToChangeList(m_kqueue.change_list, current_event->ident,
                               EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0,
                               response);
        }
        break;

        case CGI_PROCESS_TIMEOUT:  // cgi
        {
          std::cout << "=======⌛️ CGI_PROCESS_TIMEOUT ⌛️=======" << std::endl;
          int result = kill(current_event->ident, SIGTERM);
          // pipe close
          std::cout << "kill status: " << result << std::endl;
          wait(NULL);
          AddEventToChangeList(m_kqueue.change_list, current_event->ident,
                               EVFILT_PROC, EV_DELETE, 0, 0, NULL);

          std::vector<char> message = getHttpMessage();  // 인자로 response 전달
          t_response_write *response =
              new t_response_write(&message[0], message.size());
          AddEventToChangeList(m_kqueue.change_list, current_event->ident,
                               EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0,
                               response);
        }
        break;

        case CLIENT_WRITE:
        {
          std::cout << "write " << std::endl;
          t_response_write *response;
          response = static_cast<t_response_write *>(current_event->udata);

          int send_byte = 0;
          send_byte =
              send(current_event->ident, response->message + response->offset,
                   response->length - response->offset, 0);
          response->offset += send_byte;
          std::cout << "server end byte to fd : " << current_event->ident
                    << "  send_byte : " << send_byte
                    << " legnth : " << response->length
                    << "offset  : " << response->offset << std::endl;
          if (response->length > response->offset)
          {
            continue;
          }
          std::cout << "send END" << std::endl;
          AddEventToChangeList(m_kqueue.change_list, current_event->ident,
                               EVFILT_WRITE, EV_DELETE, 0, 0, NULL);
        }
        break;

        case CLIENT_ERROR:
        {
          std::cout << "client socket error" << std::endl;
          disconnect_socket(current_event->ident);
        }
        break;

        case CLIENT_EOF:
        {
          disconnect_socket(current_event->ident);
        }
        break;

        case SERVER_EOF:
        {
          disconnect_socket(current_event->ident);
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
  for (int i = 0; i < servers.size(); ++i)
  {
    shutdown(servers[i].server_sock, SHUT_RDWR);
    close(servers[i].server_sock);
  }
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
