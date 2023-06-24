#include "Server.hpp"

#include "Log.hpp"

void Server::disconnectSocket(int socket) { close(socket); }

const std::vector<t_multi_server> &Server::get_servers(void) { return servers; }

void Server::addCloseFdVector(int fd)
{
  if (m_close_udata_map.find(fd) == m_close_udata_map.end())
  {
    std::vector<t_event_udata*> vec;
    m_close_udata_map.insert(std::make_pair(fd , vec));
    std::cout << "make new vector fd :" << fd << std::endl;
  }
}

void Server::addUdataContent(int fd, t_event_udata *udata)
{
  std::map<int, std::vector<t_event_udata *> >::iterator it;
  
  if (m_close_udata_map.find(fd) == m_close_udata_map.end())
  {
    std::vector<t_event_udata*> vec;
    m_close_udata_map.insert(std::make_pair(fd , vec));
    std::cout << "make new vector fd :" << fd << std::endl;
  }
  it = m_close_udata_map.find(fd);
  if (it != m_close_udata_map.end())
  {
    it->second.push_back(udata);
  }
}

void Server::clearUdataContent(int fd, t_event_udata *udata)
{
  std::cout << "claerUdataContent" << std::endl;
  std::map<int, std::vector<t_event_udata *> >::iterator it;
  it = m_close_udata_map.find(fd);
  if (it != m_close_udata_map.end())
  {
    for (size_t i = 0; i < it->second.size(); ++i)
    {
      printf("%p %p \n",it->second[i], udata);
      if (it->second[i] == udata){
        std::cout << "in here" << std::endl;
         it->second[i] = NULL;
      }
    }
  }
}

void Server::clearUdata()
{
  std::cout << "claerUdata" << std::endl;
  for (std::set<int>::iterator i = m_close_fd_vec.begin(); i != m_close_fd_vec.end(); ++i)
  {
    std::map<int, std::vector<t_event_udata *> >::iterator it;
    it = m_close_udata_map.find(*i);
    if (it == m_close_udata_map.end())
    {
      continue;
    }

    for (size_t j = 0; j < it->second.size(); ++j)
    {
      if (it->second[j] != NULL)
      {
        printf("delete %p \n", it->second[j]);
        delete it->second[j];
        addEventToChangeList(m_kqueue.change_list, *i, EVFILT_WRITE,
                       EV_DELETE, 0, 0, NULL);
      }
    }
    it->second.clear();
    m_close_udata_map.erase(it);
    disconnectSocket(*i);
  } 
  m_close_fd_vec.clear();
  std::cout << "end" << std::endl;
}

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
    {
      LOG_DEBUG("SERVER ERROR");
      return SERVER_ERROR;
    }
    else if (type == CLIENT)
    {
      LOG_DEBUG("CLIENT ERROR");
      return CLIENT_ERROR;
    }
  }
  if (current_event->filter == EVFILT_TIMER)
  {
    {
      LOG_DEBUG("CGI TIMEOUT");
      return CGI_PROCESS_TIMEOUT;
    }
  }
  if (current_event->filter == EVFILT_READ)
  {
    if (type == SERVER)
    {
      LOG_DEBUG("SERVER READ");
      return SERVER_READ;
    }
    else if (type == CLIENT)
    {
      LOG_DEBUG("CLIENT READ");
      return CLIENT_READ;
    }
    else if (type == PIPE)
    {
      LOG_DEBUG("PIPE READ");
      return PIPE_READ;
    }
    else if (type == STATIC_FILE)
    {
      LOG_DEBUG("STATIC FILE READ");
      return STATIC_FILE_READ;
    }
  }
  if (current_event->filter == EVFILT_WRITE)
  {
    if (type == SERVER)
    {
      LOG_DEBUG("SERVER WRITE");
      return SERVER_WRITE;
    }  
    else if (type == CLIENT)
    {
      LOG_DEBUG("CLIENT WRITE");
      return CLIENT_WRITE;
    }  
    else if (type == PIPE)
    {
      LOG_DEBUG("PIPE WRITE");
      return PIPE_WRITE;
    }  
    else if (type == STATIC_FILE)
    {
      LOG_DEBUG("STATIC FILE WRITE");
      return STATIC_FILE_WRITE;
    }  
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
    catch(std::exception e)
    {
        std::cout << e.what() << std::endl;
      exit(EXIT_FAILURE);
    }
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

Server::Server(const Server &other)
{
  *this = other;
}

Server::~Server() { }

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
      ft_error_exit(1, strerror(errno));
    }
    m_kqueue.change_list.clear();
    std::cout << "cycle start --------------------------" << std::endl << std::endl;
    for (int i = 0; i < current_events; ++i)
    {
      current_event = &m_kqueue.event_list[i];
      current_udata = static_cast<t_event_udata *>(current_event->udata);

      if (current_event->flags & EV_ERROR)
      {
        printf("filter: %d ident: %d \n", current_event->filter, current_event->ident);
        std::cout << "flag is EV_ERROR: " << current_event->ident << " " << strerror(current_event->data) << std::endl;
        printf("ERROR POINTER IS %p\n", current_event->udata);
        continue;
      }
      else
      {
        printf("filter: %d ident: %d  type:%d \n", current_event->filter, current_event->ident, event_status);
        event_status = getEventStatus(current_event, current_udata->m_type);
      }
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

        default:
        {
          break;
        }
      }
    }
    std::cout << std::endl << "-------------------------- cycle END " << std::endl << std::endl;

    if (m_close_fd_vec.size() > 0)
    {
      clearUdata();
    }
  }
  for (size_t i = 0; i < servers.size(); ++i)
  {
    shutdown(servers[i].server_sock, SHUT_RDWR);
    close(servers[i].server_sock);
  }
  return;
}
