#include "Server.hpp"

#include "beforeHttpProcessor.hpp"

void Server::AddEventToChangeList(
    std::vector<struct kevent> &change_list,
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

char *Server::getHttpCharMessages(void)
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

void Server::disconnect_socket(int socket) { close(socket); }

int is_server_sock(std::vector<t_multi_server> servers, int sock)
{
  for (int i = 0; i < servers.size(); ++i)
  {
    if (servers[i].server_sock == sock) return 1;
  }
  return 0;
}

e_kqueue_event getEventStatus(struct kevent *current_event, e_event_type type)
{
  if (current_event->flags & EV_ERROR)
  {
    if (type == SERVER)
      return SERVER_ERROR;
    else if (type == CLIENT)
      return CLIENT_ERROR;
  }
  if (current_event->filter == EVFILT_TIMER)
  {
    return CGI_PROCESS_TIMEOUT;
  }
  if (current_event->filter == EVFILT_READ)
  {
    if (type == SERVER)
      return SERVER_READ;
    else if (type == CLIENT)
      return CLIENT_READ;
    else if (type == PIPE)
      return PIPE_READ;
  }
  if (current_event->filter == EVFILT_WRITE)
  {
    if (type == SERVER)
      return SERVER_WRITE;
    else if (type == CLIENT)
      return CLIENT_WRITE;
  }
  return NOTHING;
}

const std::vector<t_multi_server> &Server::get_servers(void) { return servers; }

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

  int current_events;
  e_kqueue_event event_status;
  struct kevent *current_event;
  t_event_udata *current_udata;
  for (int i = 0; i < servers.size(); ++i)
  {
    t_event_udata *udata = new t_event_udata(SERVER, servers[i].config);
    AddEventToChangeList(m_kqueue.change_list, servers[i].server_sock,
                         EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, udata);
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
      current_udata = static_cast<t_event_udata *>(current_event->udata);
      event_status = getEventStatus(current_event, current_udata->m_type);
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
