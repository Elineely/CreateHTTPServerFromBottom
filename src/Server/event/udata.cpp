#include "Server.hpp"

void Server::addCloseFdSet(int fd)
{
  if (m_close_udata_map.find(fd) == m_close_udata_map.end())
  {
    std::vector<t_event_udata *> vec;
    m_close_udata_map.insert(std::make_pair(fd, vec));
  }
}

void Server::addUdataContent(int fd, t_event_udata *udata)
{
  std::map<int, std::vector<t_event_udata *> >::iterator it;

  if (m_close_udata_map.find(fd) == m_close_udata_map.end())
  {
    std::vector<t_event_udata *> vec;
    m_close_udata_map.insert(std::make_pair(fd, vec));
  }
  it = m_close_udata_map.find(fd);
  if (it != m_close_udata_map.end())
  {
    it->second.push_back(udata);
  }
}

void Server::clearUdataContent(int fd, t_event_udata *udata)
{
  std::map<int, std::vector<t_event_udata *> >::iterator it;
  it = m_close_udata_map.find(fd);
  if (it != m_close_udata_map.end())
  {
    for (size_t i = 0; i < it->second.size(); ++i)
    {
      if (it->second[i] == udata)
      {
        it->second[i] = NULL;
      }
    }
  }
}

void Server::clearUdata()
{
  for (std::set<int>::iterator i = m_close_fd_set.begin();
       i != m_close_fd_set.end(); ++i)
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
        delete it->second[j];
        addEventToChangeList(m_kqueue.change_list, *i, EVFILT_WRITE, EV_DELETE,
                             0, 0, NULL);
      }
    }
    it->second.clear();
    m_close_udata_map.erase(it);
    disconnectSocket(*i);
  }
  m_close_fd_set.clear();
}
