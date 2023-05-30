#include "Server.hpp"

#define ERROR -1

#define CHILD_PROCESS 0

struct CustomUdata
{
  int *m_pipe_fd;
  int m_client_sock;

  CustomUdata(int *pipe_fd, int client_sock)
      : m_pipe_fd(pipe_fd), m_client_sock(client_sock)
  {
  }
};

enum
{
  READ,
  WRITE
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
          std::cout << "Client ID: " << client_sock << " is connected."
                    << std::endl;

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
          if (current_event->flags == EV_EOF)
          {
            std::cerr << "closing" << std::endl;
            disconnect_socket(current_event->ident);
            continue;
          }

          std::cout << "📖 CLIENT_READ 📖" << std::endl;
          Parser *parser = static_cast<Parser *>(current_event->udata);
          // sleep(1);0

          char buff[BUF_SIZE];
          std::memset(buff, 0, BUF_SIZE);
          int recv_size = recv(current_event->ident, buff, sizeof(buff), 0);
          parser->readBuffer(buff);
          if (parser->get_validation_phase() != COMPLETE)
          {
            continue;
          }

          pid_t pid;
          int pipe_fd[2];
          char buf[BUF_SIZE];

          pipe(pipe_fd);
          pid = fork();
          if (pid == CHILD_PROCESS)
          {
            close(pipe_fd[READ]);
            dup2(pipe_fd[WRITE], STDOUT_FILENO);
            char *script_path = "./hello.py";
            if (execve(script_path, NULL, NULL) == ERROR)
            {
              std::cerr << "execve error: " << strerror(errno) << std::endl;
              exit(EXIT_FAILURE);
            }
            exit(EXIT_SUCCESS);
          }
          else
          {
            // close(pipe_fd[WRITE]);

            CustomUdata *udata = new CustomUdata(pipe_fd, current_event->ident);
            // udata->m_pipe_fd[WRITE] = -1;
            // Set up the event structure

            // pipe_fd[READ] 를  EV_EOF 로 이벤트로 등록
            // AddEventToChangeList(m_kqueue.change_list, pipe_fd[READ],
            // EVFILT_TIMER,
            //                      EV_ADD | EV_ONESHOT, NOTE_SECONDS, 0, NULL);
            AddEventToChangeList(m_kqueue.change_list, pid, EVFILT_TIMER,
                                 EV_ADD | EV_ONESHOT, NOTE_SECONDS, 10, udata);
            AddEventToChangeList(m_kqueue.change_list, pid, EVFILT_PROC, EV_ADD,
                                 NOTE_EXIT, 0, udata);
            // delete current_event->udata;
          }

          // getttpMessage
          // char *message = getHttpMessage();

          // if (response.cgi == true)
          // {
          //  coutinue;
          // }
          // else
          // {
          // t_response_write *response =
          //     new t_response_write(message, ft_strlen(message));
          // AddEventToChangeList(m_kqueue.change_list, current_event->ident,
          //                      EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0,
          //                      response);
          // }
          // }
        }
        break;

        case CGI_PROCESS_END:  // cgi
        {
          std::cout << "CGI_PROCESS_END" << std::endl;

          std::string result;
          char buf[BUF_SIZE];
          std::memset(buf, 0, BUF_SIZE);
          CustomUdata *udata = static_cast<CustomUdata *>(current_event->udata);
          // if (udata->m_pipe_fd[READ] == -1)
          // {
          //   continue;
          // }
          waitpid(current_event->ident, NULL, 0);
          while (read(udata->m_pipe_fd[READ], buf, BUF_SIZE) > 0)
          {
            result.append(buf);
          }
          // pipe close

          // std::cout << close(udata->m_pipe_fd[READ]) << " ---------------"
          //           << udata->m_pipe_fd[READ] << std::endl;
          // udata->m_pipe_fd[READ] = -1;
          AddEventToChangeList(m_kqueue.change_list, current_event->ident,
                               EVFILT_TIMER, EV_DELETE, 0, 0, NULL);
          // char *message = getHttpMessage();  // 인자로 response 전달
          const char *message = result.c_str();

          std::cout << "🚀 CGI MESSAGE 🚀" << std::endl;
          std::cout << message << std::endl;

          t_response_write *response =
              new t_response_write(message, ft_strlen(message));
          AddEventToChangeList(m_kqueue.change_list, udata->m_client_sock,
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
          waitpid(current_event->ident, NULL, 0);
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

          std::cout << "✅ CLIENT WRITE ✅" << std::endl;
          std::cout << response->message << std::endl;

          int send_byte = 0;
          send_byte =
              send(current_event->ident, response->message + response->offset,
                   response->length - response->offset, 0);
          response->offset += send_byte;
          std::cerr << strerror(errno) << std::endl;
          std::cout << "server end byte to fd : " << current_event->ident
                    << "  send_byte : " << send_byte << std::endl;
          if (response->length > response->offset)
          {
            continue;
          }
          std::cout << "send END" << std::endl;
          AddEventToChangeList(m_kqueue.change_list, current_event->ident,
                               EVFILT_WRITE, EV_DELETE, 0, 0, response);
          free(response);
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
