#include "Log.hpp"
#include "Server.hpp"
#include "utils.hpp"

void Server::clientWriteEvent(struct kevent *current_event)
{
  LOG_INFO("✅ CLIENT WRITE EVENT ✅");

  t_event_udata *current_udata;
  t_response_write *response_write;
  char *message;

  current_udata = static_cast<t_event_udata *>(current_event->udata);
  response_write = &current_udata->m_response_write;
  message = &response_write->message[0];
  int send_byte = 0;
  send_byte = send(current_event->ident, message + response_write->offset,
                   response_write->length - response_write->offset, 0);
  response_write->offset += send_byte;
  if (response_write->length > response_write->offset)
  {
    return;
  }
  addEventToChangeList(m_kqueue.change_list, current_event->ident, EVFILT_WRITE,
                       EV_DELETE, 0, 0, NULL);
  ft_delete(&(current_udata->m_request));
  ft_delete(&(current_udata->m_response));
  ft_delete(&current_udata);
}

void Server::pipeWriteEvent(struct kevent *current_event)
{
  // LOG_INFO("🛎 PIPE WRITE EVENT 🛎");

  t_event_udata *current_udata;
  int possible_write_length;
  size_t request_body_size;
  size_t pipe_write_length;
  size_t pipe_write_offset;
  ssize_t write_byte;

  current_udata = static_cast<t_event_udata *>(current_event->udata);
  struct Request &current_request = *current_udata->m_request;
  possible_write_length = current_event->data;
  pipe_write_offset = current_udata->m_pipe_write_offset;
  request_body_size = current_request.body.size();

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

    // LOG_DEBUG("write_byte: %d", write_byte);
    if (write_byte == -1)
    {
      LOG_INFO("write error");
    }
    else
    {
      current_udata->m_pipe_write_offset += write_byte;
    }
  }

  if (current_udata->m_pipe_write_offset == request_body_size)
  {
    close(current_udata->m_write_pipe_fd);
    ft_delete(&current_udata->m_request);
    ft_delete(&current_udata->m_response);
    ft_delete(&current_udata);
  }
}
