#include "HttpProcessor.hpp"

HttpProcessor::HttpProcessor(void) {}

HttpProcessor::HttpProcessor(Request& request_data, t_server& server_data)
{
  m_request_data = request_data;
  m_server_data = server_data;
  MethodHandler* method_handler;
  PathFinder path_finder(Request request_data, t_server server_data,
                           Response& m_response_data);
  method_handler = &GetMethodHandler(m_request_data, m_config_data);
}
// Copy Constructor
HttpProcessor::HttpProcessor(const HttpProcessor& obj) {}

// Destructor
HttpProcessor::~HttpProcessor(void) {}

Copy Assignment Operator HttpProcessor& HttpProcessor::operator=(
    HttpProcessor const& obj)
{
}