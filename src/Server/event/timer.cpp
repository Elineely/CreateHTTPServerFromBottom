#include "Server.hpp"

void Server::cgiProcessTimeoutEvent(struct kevent *current_event)
{
  std::cout << "⌛️ CGI PROCESS TIMEOUT EVENT ⌛️" << std::endl;
  EventUdata *udata = static_cast<EventUdata *>(current_event->udata);
  // pipe close
  close(udata->m_pipe_read_fd);
  int result = kill(current_event->ident, SIGTERM);
  waitpid(current_event->ident, NULL, 0);

  // error 메시지로 변경 후 전달
  char* message = getHttpCharMessages();
  // char message[] =
  //     "HTTP/1.1 200 OK\r\nContent-type: text/plain\r\nContent-length: "
  //     "5\r\n\r\nhello";  // 인자로 response 전달

  t_response_write *response =
      new t_response_write(&message[0], ft_strlen(message));
  AddEventToChangeList(CLIENT, m_kqueue.change_list, udata->m_client_sock,
                       EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, response);
  m_event_fd_list.erase(udata->m_pipe_read_fd);
  m_event_fd_list.erase(udata->m_child_pid);
}