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

char *getHttpMessage()
{
  char *return_buff = (char *)malloc(1000000);
  int file_length;
  std::string file_buff;
  std::ifstream test_file("./test_message");
  std::string line;

  while (getline(test_file, line))
  {
    file_buff.append(line);
  }
  test_file.close();
  file_length = ft_strlen(file_buff.c_str());
  sprintf(return_buff,
          "HTTP/1.1 200 OK\r\nServer:Linux Web Server \r\nDate: Wed, "
          "03 May "
          "2023 05:53:31 GM \r\nContent-length:%d\r\n"
          "Content-Type:text/plain\r\n\r\n%s",
          file_length, file_buff.c_str());
  return return_buff;
}

void disconnect_socket(int socket) { close(socket); }

e_kqueue_event getEventStatus(struct kevent *current_event, int server_sock)
{
  if (current_event->flags == EV_ERROR)
  {
    if (current_event->ident == server_sock)
      return SERVER_ERROR;
    else
      return CLIENT_ERROR;
  }
  if (current_event->flags == EV_EOF)
  {
    if (current_event->ident == server_sock)
      return SERVER_EOF;
    else
      return CLIENT_EOF;
  }
  if (current_event->filter == EVFILT_READ)
  {
    if (current_event->ident == server_sock)
      return SERVER_READ;
    else
      return CLIENT_READ;
  }
  if (current_event->filter == EVFILT_WRITE)
  {
    if (current_event->ident == server_sock)
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

      event_status = getEventStatus(current_event, m_socket.server_sock);
      switch (event_status)
      {
        case SERVER_READ:
        {
          std::cout << "--- in SERVER_READ" << std::endl;

          client_sock =
              accept(m_socket.server_sock, (struct sockaddr *)&client_addr,
                     reinterpret_cast<socklen_t *>(&client_addr_size));
          if (client_sock == -1)
          {
            ft_error(1, strerror(errno));
            continue;
          }
          std::cout << "새로운 클라이언트가 연결 되었습니다." << std::endl;

          // TODO: max_body_size 는 server block 마다 다를텐데 어떻게 동적으로 설정?
          std::string max_body_size =
              server_conf.get_m_server_conf()[0].max_body_size[0];
          Parser *parser = new Parser(max_body_size);  // TODO: delete 하는 부분 추가하기
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
          int recv_size = recv(current_event->ident, buff, sizeof(buff), 0);
          parser->readBuffer(buff);
          if (parser->get_validation_phase() != COMPLETE)
          {
            continue;
          }

          // getttpMessage
          char *message = getHttpMessage();

          // if (response.cgi == true)
          // {
          //  coutinue;
          // }
          // else
          // {
          t_response_write *response =
              new t_response_write(message, ft_strlen(message));
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

          char *message = getHttpMessage();  // 인자로 response 전달

          t_response_write *response =
              new t_response_write(message, ft_strlen(message));
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

          char *message = getHttpMessage();  // 인자로 response 전달

          t_response_write *response =
              new t_response_write(message, ft_strlen(message));
          AddEventToChangeList(m_kqueue.change_list, current_event->ident,
                               EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0,
                               response);
        }
        break;

        case CLIENT_WRITE:
        {
          t_response_write *response;
          response = static_cast<t_response_write *>(current_event->udata);

          int send_byte = 0;
          send_byte =
              send(current_event->ident, response->message + response->offset,
                   response->length - response->offset, 0);
          response->offset += send_byte;
          std::cout << "server end byte to fd : " << current_event->ident
                    << "  send_byte : " << send_byte << std::endl;
          if (response->length > response->offset)
          {
            continue;
          }
          std::cout << "send END" << std::endl;
          free(response);
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
