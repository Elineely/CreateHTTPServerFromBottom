#include "MethodHandler.hpp"

#include <fstream>
#include <sstream>

MethodHandler::MethodHandler(void) {}
MethodHandler::MethodHandler(const MethodHandler& obj)
{
  m_server_config = obj.m_server_config;
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
  if (m_response_data.get_m_file_path() == "") throw NOT_FOUND_404;
  if (m_server_config.locations[m_location_name].auto_index != "")
  {
    // auto index페이지 생성
  }
  else  // 일반 페이지 생성
  {
    std::ifstream target_file_stream(m_response_data.get_m_file_path());
    std::stringstream buffer;

    if (!target_file_stream.is_open()) throw NOT_FOUND_404;
    buffer << target_file_stream.rdbuf();
    m_body = buffer.str();
    target_file_stream.close();
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
  std::ifstream target_file_stream(m_response_data.get_m_file_path());
  std::stringstream buffer;

  if (!target_file_stream.is_open())
  {
    std::ofstream new_file_stream(m_response_data.get_m_file_path());
    if (!new_file_stream) throw "Failed to create the file.";
    // new_file_stream << m_request_data.body;
    new_file_stream.write(&m_request_data.body[0], m_request_data.body.size());
    new_file_stream.close();
  }
  else
  {
    std::ofstream append_stream(m_response_data.get_m_file_path(),
                                std::ios::app);
    std::stringstream content_stream;
    // 파일스트림 실패시 어떤 에러코드를 줄지 모르겠음
    if (!append_stream) throw "Failed to open the file for appending.";
    content_stream.write(&m_request_data.body[0], m_request_data.body.size());
    append_stream << content_stream.str();
    append_stream.close();
  }
  buffer << target_file_stream.rdbuf();
  m_body = buffer.str();
  target_file_stream.close();
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
  std::ifstream target_file_stream(m_response_data.get_m_file_path());

  if (!target_file_stream.is_open())
    throw NOT_FOUND_404;
  else
  {
    target_file_stream.close();
    // 삭제 실패시 오류코드
    if (remove(m_response_data.get_m_file_path().c_str())) throw 500;
  }
}
