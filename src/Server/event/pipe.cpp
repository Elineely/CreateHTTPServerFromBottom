#include "HttpProcessor.hpp"
#include "Log.hpp"
#include "ResponseGenerator.hpp"
#include "Server.hpp"
#include "ServerFinder.hpp"

void Server::pipeReadEvent(struct kevent *current_event)
{
  char *buf;
  char temp_buf[BUF_SIZE];
  ssize_t read_byte;
  t_event_udata *current_udata;

  LOG_DEBUG("💧 PIPE READ EVENT 💧");
  current_udata = static_cast<t_event_udata *>(current_event->udata);
  read_byte = read(current_event->ident, temp_buf, BUF_SIZE);
  LOG_DEBUG("read_byte: %d", read_byte);
  
  if (read_byte == -1)
  {
    std::cerr << "pipe read_byte - 1" << std::endl;
    close(current_udata->m_write_pipe_fd);
    close(current_event->ident);
    ft_delete(&(current_udata->m_other_udata->m_request));
    ft_delete(&(current_udata->m_other_udata->m_response));
    ft_delete(&(current_udata->m_other_udata));
    ft_delete(&current_udata);
  }
  else if (read_byte > 0)
  {
    try
    {
      buf = new char[read_byte]();
    }
    catch(const std::exception& e)
    {
      std::cerr << e.what() << std::endl;
      exit(EXIT_FAILURE);
    }
    std::memmove(buf, temp_buf, read_byte);
    current_udata->m_read_buffer.push_back(buf);
    current_udata->m_read_bytes.push_back(read_byte);
    current_udata->m_total_read_byte += read_byte;
    return;
  }
  else if (current_event->flags & EV_EOF || read_byte == 0)
  {
    int status;

    waitpid(current_udata->m_child_pid, &status, 0);
    current_udata->m_response->body.reserve(current_udata->m_total_read_byte);
    for (size_t i = 0; i < current_udata->m_read_buffer.size(); ++i)
    {
      for (size_t j = 0; j < current_udata->m_read_bytes[i]; ++j)
      {
        current_udata->m_response->body.push_back(
            current_udata->m_read_buffer[i][j]);
      }
      delete current_udata->m_read_buffer[i];
    }
    close(current_udata->m_write_pipe_fd);
    close(current_event->ident);
    if (!WIFSIGNALED(status))
    {
      ResponseGenerator ok(*current_udata->m_request, *current_udata->m_response);
      t_event_udata *udata;
      try
      {
        udata = new t_event_udata(CLIENT, current_udata->m_servers);
        addUdataContent(current_udata->m_client_sock, udata);
      }
      catch(const std::exception& e)
      {
        std::cerr << e.what() << std::endl;
        exit(EXIT_FAILURE);
      }
      udata->m_response_write.message = ok.generateResponseMessage();
      udata->m_response_write.offset = 0;
      udata->m_response_write.length = udata->m_response_write.message.size();

      addEventToChangeList(m_kqueue.change_list, current_udata->m_client_sock,
                          EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, udata);
      addEventToChangeList(m_kqueue.change_list, current_udata->m_child_pid,
                          EVFILT_TIMER, EV_DELETE, 0, 0, NULL);
 addEventToChangeList(m_kqueue.change_list, current_event->ident,
                          EVFILT_READ, EV_DELETE, 0, 0, NULL);
      Log::printRequestResult(current_udata);
    }
    if (current_udata->m_write_udata != NULL)
    {
      ft_delete(&current_udata->m_write_udata->m_request);
      ft_delete(&current_udata->m_write_udata->m_response);
      ft_delete(&current_udata->m_write_udata);
    }
    ft_delete(&(current_udata->m_other_udata->m_request));
    ft_delete(&(current_udata->m_other_udata->m_response));
    ft_delete(&(current_udata->m_other_udata));
    ft_delete(&current_udata->m_request);
    ft_delete(&current_udata->m_response);
    ft_delete(&current_udata);
  }
}

void Server::pipeWriteEvent(struct kevent *current_event)
{
  t_event_udata *current_udata;
  size_t possible_write_length;
  size_t request_body_size;
  size_t file_write_length;
  size_t file_write_offset;
  ssize_t write_byte;

  LOG_DEBUG("🧪 PIPE WRITE EVENT 🧪");
  current_udata = static_cast<t_event_udata *>(current_event->udata);
  struct Request &current_request = *current_udata->m_request;
  possible_write_length = current_event->data;
  file_write_offset = current_udata->m_file_write_offset;
  request_body_size = current_request.body.size();

  if (possible_write_length > 0)
  {
    if (request_body_size - file_write_offset > possible_write_length)
    {
      file_write_length = possible_write_length;
    }
    else
    {
      file_write_length = request_body_size - file_write_offset;
    }
    write_byte =
        write(current_event->ident, &current_request.body[file_write_offset],
              file_write_length);
    if (write_byte == -1)
    {
      Log::print(ERROR, "write error");
    }
    else
    {
      current_udata->m_file_write_offset += write_byte;
    }
  }

  if (current_udata->m_file_write_offset == request_body_size)
  {
    close(current_event->ident);
  }
}
