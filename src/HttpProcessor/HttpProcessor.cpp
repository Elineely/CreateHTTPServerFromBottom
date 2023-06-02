#include "HttpProcessor.hpp"

HttpProcessor::HttpProcessor(void) {}

HttpProcessor::~HttpProcessor(void) {}

HttpProcessor::HttpProcessor(Request& request_data, t_server& server_data)
{
  m_request_data = request_data;
  m_server_data = server_data;
  MethodHandler* method_handler;
  PathFinder path_finder(Request request_data, t_server server_data,
                           Response& m_response_data);
  method_handler = &GetMethodHandler(m_request_data, m_response_data);
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
    return (*this);
  m_request_data = obj.m_request_data;
  m_response_data = obj.m_response_data;
  m_server_data = obj.m_server_data;
  return (*this);
}
