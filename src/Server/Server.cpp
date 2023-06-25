#include "Server.hpp"

#include "Log.hpp"

void Server::disconnectSocket(int socket) { close(socket); }

const std::vector<t_multi_server> &Server::get_servers(void) { return servers; }

void Server::addEventToChangeList(
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
    else if (type == STATIC_FILE)
      return STATIC_FILE_READ;
  }
  if (current_event->filter == EVFILT_WRITE)
  {
    if (type == SERVER)
      return SERVER_WRITE;
    else if (type == CLIENT)
      return CLIENT_WRITE;
    else if (type == PIPE)
      return PIPE_WRITE;
    else if (type == STATIC_FILE)
      return STATIC_FILE_WRITE;
  }
  return NOTHING;
}

void Server::addServerSocketEvent(std::vector<t_multi_server> &servers,
                                  Config &server_conf)
{
  for (size_t i = 0; i < servers.size(); ++i)
  {
    t_event_udata *udata;
    try
    {
      udata = new t_event_udata(SERVER, server_conf.get_m_server_conf());
    }
    catch (std::exception e)
    {
      std::cout << e.what() << std::endl;
      exit(EXIT_FAILURE);
    }
    addUdataMap(servers[i].server_sock, udata);
    addEventToChangeList(m_kqueue.change_list, servers[i].server_sock,
                         EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, udata);
  }
}

Server::Server(Config &server_conf)
{
  setServers(server_conf, servers);
  setSocket(servers);
  startBind(servers);
  startListen(servers, BACK_LOG);

  m_kqueue.kq = getKqueue();

  addServerSocketEvent(servers, server_conf);
}

Server::Server() {}

Server::Server(const Server &other) { *this = other; }

Server::~Server() {}

Server &Server::operator=(const Server &other)
{
  if (this != &other)
  {
    m_kqueue = other.m_kqueue;
    servers = other.servers;
  }
  return *this;
}

void Server::start(void)
{
  int current_events;
  e_kqueue_event event_status;
  struct kevent *current_event;
  t_event_udata *current_udata;

  Log::start();
  while (1)
  {
    current_events = kevent(m_kqueue.kq, &m_kqueue.change_list[0],
                            m_kqueue.change_list.size(), m_kqueue.event_list,
                            MAX_EVENT_LIST_SIZE, NULL);

    if (current_events == -1)
    {
      ft_error_exit(EXIT_FAILURE, strerror(errno));
    }

    m_kqueue.change_list.clear();
    for (int i = 0; i < current_events; ++i)
    {
      // printf("\n cycle start ------------------ \n");
      current_event = &m_kqueue.event_list[i];
      current_udata = static_cast<t_event_udata *>(current_event->udata);

      // printf("ident: %d / filter: %d / flags: %d udata: %p\n",
      //        current_event->ident, current_event->filter, current_event->flags,
      //        current_udata);
      if (current_event->flags & EV_ERROR)
      {
        // std::cout << current_event->ident << "server delete register "
        //           << std::endl;
        if (m_fd_set.find(current_event->ident) == m_fd_set.end())
        {
          continue;
        }
        m_fd_set.insert(current_event->ident);
        continue;
      }
      event_status = getEventStatus(current_event, current_udata->m_type);
      switch (event_status)
      {
      case SERVER_READ:
        {
          serverReadEvent(current_event);
          break;
        }

        case SERVER_ERROR:
        {
          serverErrorEvent(current_event);
          break;
        }

        case CLIENT_READ:
        {
          clientReadEvent(current_event);
          break;
        }

        case CLIENT_WRITE:
        {
          clientWriteEvent(current_event);
          break;
        }

        case CLIENT_ERROR:
        {
          disconnectSocket(current_event->ident);
          break;
        }

        case PIPE_READ:
        {
          pipeReadEvent(current_event);
          break;
        }

        case PIPE_WRITE:
        {
          pipeWriteEvent(current_event);
          break;
        }

        case CGI_PROCESS_TIMEOUT:  // cgi
        {
          cgiProcessTimeoutEvent(current_event);
          break;
        }

        case STATIC_FILE_READ:
        {
          staticFileReadEvent(current_event);
          break;
        }

        case STATIC_FILE_WRITE:
        {
          fileWriteEvent(current_event);
          break;
        }

        default:
        {
          break;
        }
      }
    }
    if (m_fd_set.size() > 0)
    {
      clearUdata();
    }
    // printf(" ------------------  cycle end \n\n");
  }
  for (size_t i = 0; i < servers.size(); ++i)
  {
    shutdown(servers[i].server_sock, SHUT_RDWR);
    close(servers[i].server_sock);
  }
  return;
}
