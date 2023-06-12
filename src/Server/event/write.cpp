#include "Log.hpp"
#include "Server.hpp"

void Server::clientWriteEvent(struct kevent *current_event)
{
  LOG_INFO("✅ CLIENT WRITE EVENT ✅");

  t_event_udata *udata;
  t_event_udata *new_udata;
  t_event_udata *read_udata;
  t_response_write *response_write;
  char *message;

  udata = static_cast<t_event_udata *>(current_event->udata);
  response_write = &udata->m_response_write;
  message = &response_write->message[0];
  int log_file = open("./log_file", O_WRONLY | O_CREAT | O_TRUNC, 0644);
  write(log_file, message, response_write->length);
  close(log_file);
  int send_byte = 0;
  send_byte = send(current_event->ident, message + response_write->offset,
                   response_write->length - response_write->offset, 0);
  response_write->offset += send_byte;
  if (response_write->length > response_write->offset)
  {
    return;
  }
  AddEventToChangeList(m_kqueue.change_list, current_event->ident, EVFILT_WRITE,
                       EV_DELETE, 0, 0, NULL);
  delete udata;
}

void Server::pipeWriteEvent(struct kevent *current_event)
{
  LOG_INFO("🛎 PIPE WRITE EVENT 🛎");

  t_event_udata *current_udata;
  struct Request current_request;
  int possible_write_length;
  size_t request_body_size;
  size_t pipe_write_length;
  size_t pipe_write_offset;
  ssize_t write_byte;

  current_udata = static_cast<t_event_udata *>(current_event->udata);
  current_request = current_udata->m_parser.get_request();
  possible_write_length = current_event->data;
  pipe_write_offset = current_udata->m_pipe_write_offset;
  request_body_size = current_request.body.size();

  if (current_request.method == "GET")
  {
    close(current_udata->m_write_pipe_fd);
    AddEventToChangeList(m_kqueue.change_list, current_udata->m_read_pipe_fd,
                         EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, current_udata);
    return;
  }

  if (possible_write_length > 0)
  {
    if (request_body_size - pipe_write_offset > possible_write_length)
    {
      pipe_write_length = possible_write_length;
    }
    else
    {
      pipe_write_length = request_body_size - pipe_write_offset;
    }
    write_byte =
        write(current_udata->m_write_pipe_fd,
              &current_request.body[pipe_write_offset], pipe_write_length);

    char buf[BUF_SIZE];
    // std::memset(buf, 0, BUF_SIZE);
    // ssize_t read_byte = read(current_udata->m_read_pipe_fd, buf, BUF_SIZE);
    // write(1, buf, BUF_SIZE);

    if (write_byte == -1)
    {
      LOG_ERROR("write error");
    }
    else
    {
      current_udata->m_pipe_write_offset += write_byte;
    }
  }

  if (current_udata->m_pipe_write_offset == request_body_size)
  {
    close(current_udata->m_write_pipe_fd);
    AddEventToChangeList(m_kqueue.change_list, current_udata->m_read_pipe_fd,
                         EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, current_udata);
  }
  // 덜 보냈으면 자동 return
}
