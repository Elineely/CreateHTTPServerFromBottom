#include "Server.hpp"

#include "beforeHttpProcessor.hpp"

#define ERROR -1

#define CHILD_PROCESS 0

struct CustomUdata
{
  int m_pipe_read_fd;
  int m_client_sock;
  pid_t m_child_pid;
  std::string result;

  CustomUdata(int pipe_read_fd, int client_sock, pid_t pid)
      : m_pipe_read_fd(pipe_read_fd),
        m_client_sock(client_sock),
        m_child_pid(pid)
  {
  }
};

enum
{
  READ,
  WRITE
};

void Server::AddEventToChangeList(
    e_event_type fd_type, std::vector<struct kevent> &change_list,
    uintptr_t ident, /* identifier for this event */
    int16_t filter,  /* filter for event */
    uint16_t flags,  /* general flags */
    uint32_t fflags, /* filter-specific flags */
    intptr_t data,   /* filter-specific data */
    void *udata)
{
  struct kevent temp_event;

  EV_SET(&temp_event, ident, filter, flags, fflags, data, udata);
  m_event_fd_list.insert(std::pair<int, e_event_type>(ident, fd_type));
  change_list.push_back(temp_event);
}

char *getHttpMessages()
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

void Server::serverReadEvent(struct kevent *current_event)
{
  std::cout << "🖥  SERVER READ EVENT  🖥" << std::endl;
  int client_sock;
  int client_addr_size;
  struct sockaddr_in client_addr;

  client_sock = accept(current_event->ident, (struct sockaddr *)&client_addr,
                       reinterpret_cast<socklen_t *>(&client_addr_size));
  if (client_sock == -1)
  {
    ft_error(1, strerror(errno));
    return;
  }
  std::cout << "🏃 Client ID: " << client_sock << " is connected. 🏃"
            << std::endl;

  std::string max_body_size =
      static_cast<t_server *>(current_event->udata)->max_body_size[0];
  // TODO: delete 하는 부분 추가하기
  Parser *parser = new Parser(max_body_size, client_sock);
  fcntl(client_sock, F_SETFL, O_NONBLOCK);
  AddEventToChangeList(CLIENT, m_kqueue.change_list, client_sock, EVFILT_READ,
                       EV_ADD | EV_ENABLE, 0, 0, parser);
  m_kqueue.socket_clients[client_sock] = "";
}

void Server::pipeReadEvent(struct kevent *current_event)
{
  std::cout << "💧 PIPE READ EVENT 💧" << std::endl;
  std::cout << "current_event->ident: " << current_event->ident << std::endl;

  char buf[BUF_SIZE];
  std::memset(buf, 0, BUF_SIZE);
  CustomUdata *udata = static_cast<CustomUdata *>(current_event->udata);
  std::cout << "udata->m_client_sock: " << udata->m_client_sock << std::endl;

  ssize_t read_byte = read(udata->m_pipe_read_fd, buf, BUF_SIZE);
  std::cout << "read result: " << read_byte << std::endl;
  if (read_byte > 0)
  {
    udata->result.append(buf);
    return;
  }
  wait(NULL);
  if (current_event->flags & EV_EOF)
  {
    std::cout << "💩 PIPE EOF EVENT 💩" << std::endl;
    close(udata->m_pipe_read_fd);

    // char *message = getHttpMessage();  // 인자로 response 전달
    const char *message = udata->result.c_str();

    t_response_write *response =
        new t_response_write(message, ft_strlen(message));
    AddEventToChangeList(CLIENT, m_kqueue.change_list, udata->m_client_sock,
                         EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, response);
    AddEventToChangeList(PROCESS, m_kqueue.change_list, udata->m_child_pid,
                         EVFILT_TIMER, EV_DELETE, 0, 0, NULL);
    m_event_fd_list.erase(udata->m_pipe_read_fd);
    m_event_fd_list.erase(udata->m_child_pid);  // EVFILT_TIMER
  }
}

void Server::cgiProcessTimeoutEvent(struct kevent *current_event)
{
  std::cout << "⌛️ CGI PROCESS TIMEOUT EVENT ⌛️" << std::endl;
  CustomUdata *udata = static_cast<CustomUdata *>(current_event->udata);
  // pipe close
  close(udata->m_pipe_read_fd);
  int result = kill(current_event->ident, SIGTERM);
  waitpid(current_event->ident, NULL, 0);

  char message[] =
      "HTTP/1.1 200 OK\r\nContent-type: text/plain\r\nContent-length: "
      "5\r\n\r\nhello";  // 인자로 response 전달

  t_response_write *response =
      new t_response_write(message, ft_strlen(message));
  AddEventToChangeList(CLIENT, m_kqueue.change_list, udata->m_client_sock,
                       EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, response);
  m_event_fd_list.erase(udata->m_pipe_read_fd);
  m_event_fd_list.erase(udata->m_child_pid);
}

void Server::serverErrorEvent(struct kevent *current_event)
{
  std::cout << "server socket error" << std::endl;
  disconnect_socket(current_event->ident);
}

void Server::clientWriteEvent(struct kevent *current_event)
{
  std::cout << "✅ CLIENT WRITE EVENT ✅" << std::endl;

  t_response_write *response;
  response = static_cast<t_response_write *>(current_event->udata);

  int send_byte = 0;
  send_byte = send(current_event->ident, response->message + response->offset,
                   response->length - response->offset, 0);
  response->offset += send_byte;
  if (response->length > response->offset)
  {
    return;
  }
  AddEventToChangeList(CLIENT, m_kqueue.change_list, current_event->ident,
                       EVFILT_WRITE, EV_DELETE, 0, 0, response);
  // free(response);
}

void Server::clientReadEvent(struct kevent *current_event)
{
  std::cout << "📖 CLIENT_READ EVENT 📖" << std::endl;
  if (current_event->flags & EV_EOF)
  {
    std::cerr << "closing" << std::endl;
    disconnect_socket(current_event->ident);
    return;
  }

  Parser *parser = static_cast<Parser *>(current_event->udata);

  char buff[BUF_SIZE];
  std::memset(buff, 0, BUF_SIZE);
  int recv_size = recv(current_event->ident, buff, sizeof(buff), 0);
  parser->readBuffer(buff);
  if (parser->get_validation_phase() != COMPLETE)
  {
    return;
  }

  pid_t pid;
  int pipe_fd[2];
  char buf[BUF_SIZE];

  pipe(pipe_fd);
  // TODO: 테스터기 돌릴 때 생존 여부 확인하기
  fcntl(pipe_fd[READ], F_SETFL, O_NONBLOCK);
  pid = fork();
  char *argv[] = {"/usr/bin/python3", "./hello.py", NULL};
  char *envp[] = {NULL};
  if (pid == CHILD_PROCESS)
  {
    close(pipe_fd[READ]);
    dup2(pipe_fd[WRITE], STDOUT_FILENO);
    char *script_path = "/usr/bin/python3";
    if (execve(script_path, argv, envp) == ERROR)
    {
      std::cerr << "execve error: " << strerror(errno) << std::endl;
      exit(EXIT_FAILURE);
    }
  }
  else
  {
    close(pipe_fd[WRITE]);
    CustomUdata *udata =
        new CustomUdata(pipe_fd[READ], current_event->ident, pid);
    // udata->m_pipe_fd[WRITE] = -1;
    // Set up the event structure

    // pipe_fd[READ] 를  EV_EOF 로 이벤트로 등록
    AddEventToChangeList(PROCESS, m_kqueue.change_list, pid, EVFILT_TIMER,
                         EV_ADD | EV_ONESHOT, NOTE_SECONDS, 2, udata);
    AddEventToChangeList(PIPE, m_kqueue.change_list, pipe_fd[READ], EVFILT_READ,
                         EV_ADD | EV_ENABLE, 0, 0, udata);
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

void Server::disconnect_socket(int socket) { close(socket); }

int is_server_sock(std::vector<t_multi_server> servers, int sock)
{
  for (int i = 0; i < servers.size(); ++i)
  {
    if (servers[i].server_sock == sock) return 1;
  }
  return 0;
}

e_kqueue_event getEventStatus(struct kevent *current_event,
                              std::map<int, e_event_type> &m_event_fd_list)
{
  std::map<int, e_event_type>::iterator it;

  it = m_event_fd_list.find(current_event->ident);
  if (it == m_event_fd_list.end()) return NOTHING;

  e_event_type fd_type = it->second;
  if (current_event->flags & EV_ERROR)
  {
    if (fd_type == SERVER)
      return SERVER_ERROR;
    else if (fd_type == CLIENT)
      return CLIENT_ERROR;
  }
  if (current_event->filter == EVFILT_TIMER)
  {
    return CGI_PROCESS_TIMEOUT;
  }
  if (current_event->filter == EVFILT_READ)
  {
    if (fd_type == SERVER)
      return SERVER_READ;
    else if (fd_type == CLIENT)
      return CLIENT_READ;
    else if (fd_type == PIPE)
      return PIPE_READ;
  }
  if (current_event->filter == EVFILT_WRITE)
  {
    if (fd_type == SERVER)
      return SERVER_WRITE;
    else if (fd_type == CLIENT)
      return CLIENT_WRITE;
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
    AddEventToChangeList(SERVER, m_kqueue.change_list, servers[i].server_sock,
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

      event_status = getEventStatus(current_event, m_event_fd_list);
      switch (event_status)
      {
        case SERVER_READ:
        {
          serverReadEvent(current_event);
        }
        break;

        case SERVER_ERROR:
        {
          serverErrorEvent(current_event);
        }
        break;

        case CLIENT_READ:
        {
          clientReadEvent(current_event);
        }
        break;

        case PIPE_READ:
        {
          pipeReadEvent(current_event);
        }
        break;

          // case PIPE_EOF:  // cgi end
          // {
          //   pipeEOFevent(current_event);
          // }
          // break;

        case CGI_PROCESS_TIMEOUT:  // cgi
        {
          cgiProcessTimeoutEvent(current_event);
        }
        break;

        case CLIENT_WRITE:
        {
          clientWriteEvent(current_event);
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
          std::cout << "deafult status == " << event_status << std::endl;
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
