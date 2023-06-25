#include "Server.hpp"

void Server::addUdataMap(int fd, t_event_udata *udata)
{
  if (m_udata_map.find(fd) == m_udata_map.end())
  {
    std::set<t_event_udata *> udata_set;

    m_udata_map.insert(std::make_pair(fd, udata_set));
  }
  std::map<int, std::set<t_event_udata *> >::iterator it;

  it = m_udata_map.find(fd);
  it->second.insert(udata);
}

void Server::removeUdata(int fd, t_event_udata *udata)
{
  std::map<int, std::set<t_event_udata *> >::iterator it;

  it = m_udata_map.find(fd);
  if (it == m_udata_map.end())
  {
    return;
  }
  std::set<t_event_udata *> &udata_set = it->second;
  std::set<t_event_udata *>::iterator udata_it;

  udata_it = udata_set.find(udata);
  if (udata_it == udata_set.end())
  {
    return;
  }

  udata_set.erase(udata_it);
}

void Server::clearUdata(void)
{
  int fd;
  std::set<int>::iterator it;
  t_event_udata *udata;
  std::map<int, std::set<t_event_udata *> >::iterator map_it;
  std::set<t_event_udata *>::iterator udata_it;

  it = m_fd_set.begin();
  for (; it != m_fd_set.end(); ++it)
  {
    fd = *it;
    map_it = m_udata_map.find(fd);
    if (map_it == m_udata_map.end())
    {
      continue;
    }
    std::set<t_event_udata *> &udata_set = map_it->second;
    udata_it = udata_set.begin();
    for (; udata_it != udata_set.end(); ++udata_it)
    {
      udata = *udata_it;
      if (udata->m_read_buffer.size() > 0)
      {
        std::vector<char *>::iterator vec_it;

        vec_it = udata->m_read_buffer.begin();
        for (; vec_it != udata->m_read_buffer.end(); ++vec_it)
        {
          delete *vec_it;
        }
        udata->m_read_buffer.clear();
      }
      if (udata->m_response->static_read_file_fd != -1)
      {
        close(udata->m_response->static_read_file_fd);
      }
      if (udata->m_child_pid != -1)
      {
        Log::print(INFO, "kill child pid: %d", udata->m_child_pid);
        kill(udata->m_child_pid, SIGTERM);
        waitpid(udata->m_child_pid, NULL, 0);
        addEventToChangeList(m_kqueue.change_list, udata->m_child_pid,
                             EVFILT_TIMER, EV_DELETE, 0, 0, NULL);
      }
      if (udata->m_read_pipe_fd != -1)
      {
        close(udata->m_read_pipe_fd);
      }
      if (udata->m_write_pipe_fd != -1)
      {
        close(udata->m_write_pipe_fd);
      }
      // std::cout << fd << " : fd ";

      // printf("delete %p\n", udata);
      ft_delete(&udata->m_request);
      ft_delete(&udata->m_response);
      ft_delete(&udata);
    }
    disconnectSocket(fd);
    udata_set.clear();
  }
  m_fd_set.clear();
}
