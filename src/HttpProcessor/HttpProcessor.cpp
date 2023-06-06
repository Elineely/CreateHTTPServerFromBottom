#include "HttpProcessor.hpp"

#include "CgiHandler.hpp"
#include "Log.hpp"

HttpProcessor::HttpProcessor(void) {}

HttpProcessor::~HttpProcessor(void) {}

HttpProcessor::HttpProcessor(Request& request_data, t_server& server_data)
{
  m_request_data = request_data;
  m_server_data = server_data;
  // MethodHandler* method_handler;
  if (request_data.status == NO_PROBLEM) request_data.status = OK_200;
  try
  {
    if (request_data.status != OK_200)
    {
      throw request_data.status;
    }
  }
  catch (StatusCode code_num)
  {
    m_response_data.status_code = code_num;
    return;
  }
  PathFinder path_finder(request_data, server_data, m_response_data);
  try
  {
    if (m_request_data.method == "GET")
    {
      if (m_response_data.cgi_flag == true)
      {
        GetCgiHandler cgi_handler(m_request_data, m_response_data);
        cgi_handler.outsourceCgiRequest();
        // m_response_data = cgi_handler.get_m_response_data();
      }
      else
      {
        GetMethodHandler method_handler(m_request_data, m_response_data);
        method_handler.methodRun();
        m_response_data = method_handler.get_m_response_data();
      }
    }
    else if (m_request_data.method == "POST")
    {
      if (m_response_data.cgi_flag == true)
      {
        PostCgiHandler cgi_handler(m_request_data, m_response_data);
        cgi_handler.outsourceCgiRequest();
        // m_response_data = cgi_handler.get_m_response_data();
      }
      else
      {
        PostMethodHandler method_handler(m_request_data, m_response_data);
        method_handler.methodRun();
        m_response_data = method_handler.get_m_response_data();
      }
    }
    else if (m_request_data.method == "PUT")
    {
      PutMethodHandler method_handler(m_request_data, m_response_data);
      method_handler.methodRun();
      m_response_data = method_handler.get_m_response_data();
    }
    else if (m_request_data.method == "HEAD")
    {
      // HeadMethodHandler method_handler(m_request_data, m_response_data);
      // method_handler.methodRun();
      // m_response_data = method_handler.get_m_response_data();
    }
    else if (m_request_data.method == "DELETE")
    {
      DeleteMethodHandler method_handler(m_request_data, m_response_data);
      method_handler.methodRun();
      m_response_data = method_handler.get_m_response_data();
    }
    else
    {
      throw METHOD_NOT_ALLOWED_405;
    }
  }
  catch (StatusCode code_num)
  {
    LOG_ERROR("[HttpProcessor] constructor catches error (status code: %d)",
              code_num);
    m_response_data.status_code = code_num;
  }
}

HttpProcessor::HttpProcessor(const HttpProcessor& obj)
{
  m_request_data = obj.m_request_data;
  m_response_data = obj.m_response_data;
  m_server_data = obj.m_server_data;
}

HttpProcessor& HttpProcessor::operator=(HttpProcessor const& obj)
{
  if (this == &obj)
  {
    return (*this);
  }
  m_request_data = obj.m_request_data;
  m_response_data = obj.m_response_data;
  m_server_data = obj.m_server_data;
  return (*this);
}

struct Response& HttpProcessor::get_m_response(void)
{
  return (m_response_data);
}
