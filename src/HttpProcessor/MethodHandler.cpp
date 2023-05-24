#include "MethodHandler.hpp"

// Abstract MethodHandler
MethodHandler::MethodHandler(void) {}
MethodHandler::MethodHandler(const MethodHandler& obj)
{
  m_config_data = obj.m_config_data;
  m_request_data = obj.m_request_data;
  m_response_data = obj.m_response_data;
  m_body = obj.m_body;
}
MethodHandler::MethodHandler(Request& request_data, Response& response_data,
                             t_server& server_config)
{
  m_request_data = request_data;
  m_response_data = response_data;
  m_server_config = server_config;
}
MethodHandler& MethodHandler::operator=(MethodHandler const& obj)
{
  if (this != &obj)
  {
    m_server_config = obj.m_server_config;
    m_request_data = obj.m_request_data;
    m_response_data = obj.m_response_data;
    m_body = obj.m_body;
  }
  return (*this);
}
MethodHandler::~MethodHandler(void) {}

// GetMethodHandler
GetMethodHandler::GetMethodHandler(void) {}
GetMethodHandler::GetMethodHandler(const GetMethodHandler& obj)
{
  m_server_config = obj.m_server_config;
  m_request_data = obj.m_request_data;
  m_response_data = obj.m_response_data;
  m_body = obj.m_body;
}
GetMethodHandler::GetMethodHandler(Request& request_data,
                                   Response& response_data,
                                   t_server& server_config)
{
  m_request_data = request_data;
  m_response_data = response_data;
  m_server_config = server_config;
}
GetMethodHandler::~GetMethodHandler(void) {}
GetMethodHandler& GetMethodHandler::operator=(GetMethodHandler const& obj)
{
  if (this != &obj)
  {
    m_request_data = obj.m_request_data;
    m_response_data = obj.m_response_data;
    m_server_config = obj.m_server_config;
    m_body = obj.m_body;
  }
  return (*this);
}
void GetMethodHandler::makeBody()
{
  if (m_server_config.locations[m_location_name].auto_index != "" &&
      m_response_data.get_m_file_path() != "")
  {
    // auto index페이지 생성
  }
  else
  {
    // 일반 페이지 생성
  }
}

// PostMethodHandler
PostMethodHandler::PostMethodHandler(void) {}
PostMethodHandler::PostMethodHandler(const PostMethodHandler& obj)
{
  m_server_config = obj.m_server_config;
  m_request_data = obj.m_request_data;
  m_response_data = obj.m_response_data;
  m_body = obj.m_body;
}
PostMethodHandler::PostMethodHandler(Request& request_data,
                                     Response& response_data,
                                     t_server& server_config)
{
  m_request_data = request_data;
  m_response_data = response_data;
  m_server_config = server_config;
}
PostMethodHandler::~PostMethodHandler(void) {}
PostMethodHandler& PostMethodHandler::operator=(PostMethodHandler const& obj)
{
  if (this != &obj)
  {
    m_request_data = obj.m_request_data;
    m_response_data = obj.m_response_data;
    m_server_config = obj.m_server_config;
    m_body = obj.m_body;
  }
  return (*this);
}
void PostMethodHandler::makeBody()
{
  if (m_server_config.locations[m_location_name].auto_index != "" &&
      m_response_data.get_m_file_path() != "")
  {
    // auto index페이지 생성
  }
  else
  {
    // 일반 페이지 생성
  }
}

// DeleteMethodHandler
DeleteMethodHandler::DeleteMethodHandler(void) {}
DeleteMethodHandler::DeleteMethodHandler(const DeleteMethodHandler& obj)
{
  m_server_config = obj.m_server_config;
  m_request_data = obj.m_request_data;
  m_response_data = obj.m_response_data;
  m_body = obj.m_body;
}
DeleteMethodHandler::DeleteMethodHandler(Request& request_data,
                                         Response& response_data,
                                         t_server& server_config)
{
  m_request_data = request_data;
  m_response_data = response_data;
  m_server_config = server_config;
}
DeleteMethodHandler::~DeleteMethodHandler(void) {}
DeleteMethodHandler& DeleteMethodHandler::operator=(
    DeleteMethodHandler const& obj)
{
  if (this != &obj)
  {
    m_request_data = obj.m_request_data;
    m_response_data = obj.m_response_data;
    m_server_config = obj.m_server_config;
    m_body = obj.m_body;
  }
  return (*this);
}
void DeleteMethodHandler::makeBody()
{
  if (m_server_config.locations[m_location_name].auto_index != "" &&
      m_response_data.get_m_file_path() != "")
  {
    // auto index페이지 생성
  }
  else
  {
    // 일반 페이지 생성
  }
}
