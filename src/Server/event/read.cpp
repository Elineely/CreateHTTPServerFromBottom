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

  fcntl(client_sock, F_SETFL, O_NONBLOCK);
  t_event_udata *udata = new t_event_udata(CLIENT);
  AddEventToChangeList(m_kqueue.change_list, client_sock, EVFILT_READ,
                       EV_ADD | EV_ENABLE, 0, 0, udata);
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

  t_event_udata *udata = static_cast<t_event_udata *>(current_event->udata);

  char buff[BUF_SIZE];
  std::memset(buff, 0, BUF_SIZE);
  int recv_size = recv(current_event->ident, buff, sizeof(buff), 0);
  udata->m_parser.readBuffer(buff);
  if (udata->m_parser.get_validation_phase() != COMPLETE)
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

    // Set up the event structure
    t_event_udata *udata = new t_event_udata(PIPE);
    t_event_udata *udata2 = new t_event_udata(PROCESS);
    udata->m_pipe_read_fd = pipe_fd[READ];
    udata->m_child_pid = pid;
    udata->m_client_sock = current_event->ident;
    udata2->m_pipe_read_fd = pipe_fd[READ];
    udata2->m_child_pid = pid;
    udata2->m_client_sock = current_event->ident;

    AddEventToChangeList(m_kqueue.change_list, pid, EVFILT_TIMER,
                         EV_ADD | EV_ONESHOT, NOTE_SECONDS, 2, udata2);
    AddEventToChangeList(m_kqueue.change_list, pipe_fd[READ], EVFILT_READ,
                         EV_ADD | EV_ENABLE, 0, 0, udata);
  }
}

void Server::pipeReadEvent(struct kevent *current_event)
{
  std::cout << "💧 PIPE READ EVENT 💧" << std::endl;

  char buf[BUF_SIZE];
  std::memset(buf, 0, BUF_SIZE);
  t_event_udata *current_udata =
      static_cast<t_event_udata *>(current_event->udata);

  ssize_t read_byte = read(current_udata->m_pipe_read_fd, buf, BUF_SIZE);
  if (read_byte > 0)
  {
    current_udata->m_result.append(buf);
    return;
  }
  wait(NULL);
  if (current_event->flags & EV_EOF)
  {
    std::cout << "💩 PIPE EOF EVENT 💩" << std::endl;
    close(current_event->ident);

    // char *message = getHttpMessage();  // 인자로 response 전달
    const char *message = current_udata->m_result.c_str();

    t_event_udata *udata =
        new t_event_udata(CLIENT, message, ft_strlen(message));

    AddEventToChangeList(m_kqueue.change_list, current_udata->m_client_sock,
                         EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, udata);
    AddEventToChangeList(m_kqueue.change_list, current_udata->m_child_pid,
                         EVFILT_TIMER, EV_DELETE, 0, 0, NULL);
  }
}
