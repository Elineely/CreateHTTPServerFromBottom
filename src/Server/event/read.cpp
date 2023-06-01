#include "Server.hpp"

#define CHILD_PROCESS 0

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
  EventUdata *udata = static_cast<EventUdata *>(current_event->udata);
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

void Server::clientReadEvent(struct kevent *current_event)
{
  std::cout << "📖 CLIENT_READ EVENT 📖" << std::endl;
  if (current_event->flags & EV_EOF)
  {
    std::cerr << "closing" << std::endl;
    disconnect_socket(current_event->ident);
    m_event_fd_list.erase(current_event->ident);
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
    EventUdata *udata =
        new EventUdata(pipe_fd[READ], current_event->ident, pid);
    // Set up the event structure

    // pipe_fd[READ] 를  EV_EOF 로 이벤트로 등록
    AddEventToChangeList(PROCESS, m_kqueue.change_list, pid, EVFILT_TIMER,
                         EV_ADD | EV_ONESHOT, NOTE_SECONDS, 2, udata);
    AddEventToChangeList(PIPE, m_kqueue.change_list, pipe_fd[READ], EVFILT_READ,
                         EV_ADD | EV_ENABLE, 0, 0, udata);
  }
}