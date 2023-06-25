#include "HttpProcessor.hpp"
#include "Log.hpp"
#include "ResponseGenerator.hpp"
#include "Server.hpp"
#include "ServerFinder.hpp"

void Server::addStaticRequestEvent(struct kevent *current_event,
                                   t_event_udata *current_udata,
                                   struct Request &request,
                                   struct Response &response)
{
  // GET, HEAD, DELETE
  if (response.static_read_file_fd != -1)
  {
    off_t file_size;

    file_size = lseek(response.static_read_file_fd, 0, SEEK_END);
    if (file_size == 0)
    {
      close(response.static_read_file_fd);
      ResponseGenerator response_generator(request, response);

      current_udata->m_response_write.message =
          response_generator.generateResponseMessage();
      current_udata->m_response_write.offset = 0;
      current_udata->m_response_write.length =
          current_udata->m_response_write.message.size();
      addEventToChangeList(m_kqueue.change_list, current_event->ident,
                           EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0,
                           current_udata);
      Log::printRequestResult(current_udata);
      return;
    }

    lseek(response.static_read_file_fd, 0, SEEK_SET);
    response.static_read_file_size = file_size;
    response.body.reserve(file_size);
    current_udata->m_type = STATIC_FILE;
    addEventToChangeList(m_kqueue.change_list, response.static_read_file_fd,
                         EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, current_udata);
  }
  // POST, PUT
  else if (response.static_write_file_fd != -1)
  {
    current_udata->m_type = STATIC_FILE;
    addEventToChangeList(m_kqueue.change_list, response.static_write_file_fd,
                         EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, current_udata);
  }
  // auto_index
  else
  {
    ResponseGenerator response_generator(request, response);

    current_udata->m_type = CLIENT;
    current_udata->m_response_write.message =
        response_generator.generateResponseMessage();
    current_udata->m_response_write.offset = 0;
    current_udata->m_response_write.length =
        current_udata->m_response_write.message.size();

    addEventToChangeList(m_kqueue.change_list, current_event->ident,
                         EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, current_udata);
    Log::printRequestResult(current_udata);
  }
}

void Server::staticFileReadEvent(struct kevent *current_event)
{
  ssize_t read_byte;
  char buf[BUF_SIZE];
  t_event_udata *current_udata;

  current_udata = static_cast<t_event_udata *>(current_event->udata);
  read_byte = read(current_event->ident, buf, BUF_SIZE);
  if (read_byte == -1)
  {
    m_fd_set.insert(current_udata->m_client_sock);
    return;
  }
  else if (read_byte > 0)
  {
    for (ssize_t idx = 0; idx < read_byte; ++idx)
    {
      current_udata->m_response->body.push_back(buf[idx]);
    }
  }
  if (current_udata->m_response->body.size() ==
      static_cast<size_t>(current_udata->m_response->static_read_file_size))
  {
    close(current_event->ident);
    Request *request = current_udata->m_request;
    Response *response = current_udata->m_response;
    ResponseGenerator response_generator(*request, *response);

    current_udata->m_response->static_read_file_fd = -1;
    current_udata->m_type = CLIENT;
    current_udata->m_response_write.message =
        response_generator.generateResponseMessage();
    current_udata->m_response_write.offset = 0;
    current_udata->m_response_write.length =
        current_udata->m_response_write.message.size();

    Log::printRequestResult(current_udata);
    addEventToChangeList(m_kqueue.change_list, current_udata->m_client_sock,
                         EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, current_udata);
  }
}

void Server::fileWriteEvent(struct kevent *current_event)
{
  t_event_udata *current_udata;
  int possible_write_length;
  size_t request_body_size;
  size_t file_write_length;
  size_t file_write_offset;
  ssize_t write_byte;

  current_udata = static_cast<t_event_udata *>(current_event->udata);
  struct Request &current_request = *current_udata->m_request;
  possible_write_length = current_event->data;
  file_write_offset = current_udata->m_file_write_offset;
  request_body_size = current_request.body.size();

  if (possible_write_length > 0)
  {
    file_write_length = request_body_size - file_write_offset;
    write_byte =
        write(current_event->ident, &current_request.body[file_write_offset],
              file_write_length);
    if (write_byte == -1)
    {
      m_fd_set.insert(current_udata->m_client_sock);
      Log::print(ERROR, "write error");
      return;
    }
    else
    {
      current_udata->m_file_write_offset += write_byte;
    }
  }

  if (current_udata->m_file_write_offset == request_body_size)
  {
    ResponseGenerator response_generator(*current_udata->m_request,
                                         *current_udata->m_response);
    current_udata->m_type = CLIENT;
    current_udata->m_response->static_write_file_fd = -1;
    current_udata->m_response_write.message =
        response_generator.generateResponseMessage();
    current_udata->m_response_write.offset = 0;
    current_udata->m_response_write.length =
        current_udata->m_response_write.message.size();

    addEventToChangeList(m_kqueue.change_list, current_udata->m_client_sock,
                         EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, current_udata);
    Log::printRequestResult(current_udata);
    close(current_event->ident);
  }
}
