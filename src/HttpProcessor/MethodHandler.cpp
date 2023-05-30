#include "MethodHandler.hpp"

#include <cstdio>
#include <fstream>
#include <sstream>

void MethodHandler::fileToBody(std::string target_file)
{
  std::ifstream file(target_file, std::ios::binary);
  if (!file.is_open()) throw INTERNAL_SERVER_ERROR_500;

  // Determine the file size, resize the vector
  file.seekg(0, std::ios::end);
  std::streampos file_size = file.tellg();
  file.seekg(0, std::ios::beg);
  std::vector<char> buffer(file_size);

  // Read the file contents into the vector
  file.read(&buffer[0], file_size);
  file.close();

  // contents of buffer into body
  m_response_data.body = buffer;
}

Request MethodHandler::get_m_request_data() { return (m_request_data); }
Response MethodHandler::get_m_response_data() { return (m_response_data); }

MethodHandler::MethodHandler(void) {}
MethodHandler::MethodHandler(const MethodHandler& obj)
{
  m_request_data = obj.m_request_data;
  m_response_data = obj.m_response_data;
}
MethodHandler::MethodHandler(Request& request_data, Response& response_data)
{
  m_request_data = request_data;
  m_response_data = response_data;
}
MethodHandler& MethodHandler::operator=(MethodHandler const& obj)
{
  if (this != &obj)
  {
    m_request_data = obj.m_request_data;
    m_response_data = obj.m_response_data;
  }
  return (*this);
}
MethodHandler::~MethodHandler(void) {}

// GetMethodHandler
GetMethodHandler::GetMethodHandler(void) {}
GetMethodHandler::GetMethodHandler(const GetMethodHandler& obj)
{
  m_request_data = obj.m_request_data;
  m_response_data = obj.m_response_data;
}
GetMethodHandler::GetMethodHandler(Request& request_data,
                                   Response& response_data)
{
  m_request_data = request_data;
  m_response_data = response_data;
}
GetMethodHandler::~GetMethodHandler(void) {}
GetMethodHandler& GetMethodHandler::operator=(GetMethodHandler const& obj)
{
  if (this != &obj)
  {
    m_request_data = obj.m_request_data;
    m_response_data = obj.m_response_data;
  }
  return (*this);
}
void GetMethodHandler::methodRun()
{
  if (!m_response_data.file_exist) throw NOT_FOUND_404;
  if (m_response_data.auto_index == true)
  {
    // auto index페이지 생성
  }
  else  // 일반 페이지 생성
    fileToBody(m_response_data.file_path + m_response_data.file_name);
}

// PostMethodHandler
PostMethodHandler::PostMethodHandler(void) {}
PostMethodHandler::PostMethodHandler(const PostMethodHandler& obj)
{
  m_request_data = obj.m_request_data;
  m_response_data = obj.m_response_data;
}
PostMethodHandler::PostMethodHandler(Request& request_data,
                                     Response& response_data)
{
  m_request_data = request_data;
  m_response_data = response_data;
}
PostMethodHandler::~PostMethodHandler(void) {}
PostMethodHandler& PostMethodHandler::operator=(PostMethodHandler const& obj)
{
  if (this != &obj)
  {
    m_request_data = obj.m_request_data;
    m_response_data = obj.m_response_data;
  }
  return (*this);
}
void PostMethodHandler::methodRun()
{
  std::string target_file(m_response_data.file_path +
                          m_response_data.file_name);

  // delete the target file
  if (m_response_data.file_exist == true)
  {
    // error deleting file
    if (std::remove(&target_file[0]) != 0) throw INTERNAL_SERVER_ERROR_500;
  }
  std::ofstream new_file_stream(target_file, std::ios::binary);
  if (!new_file_stream) throw INTERNAL_SERVER_ERROR_500;
  new_file_stream.write(&m_request_data.body[0], m_request_data.body.size());
  new_file_stream.close();
}

// DeleteMethodHandler
DeleteMethodHandler::DeleteMethodHandler(void) {}
DeleteMethodHandler::DeleteMethodHandler(const DeleteMethodHandler& obj)
{
  m_request_data = obj.m_request_data;
  m_response_data = obj.m_response_data;
}
DeleteMethodHandler::DeleteMethodHandler(Request& request_data,
                                         Response& response_data)
{
  m_request_data = request_data;
  m_response_data = response_data;
}
DeleteMethodHandler::~DeleteMethodHandler(void) {}
DeleteMethodHandler& DeleteMethodHandler::operator=(
    DeleteMethodHandler const& obj)
{
  if (this != &obj)
  {
    m_request_data = obj.m_request_data;
    m_response_data = obj.m_response_data;
  }
  return (*this);
}
void DeleteMethodHandler::methodRun()
{
  std::string target_file(m_response_data.file_path +
                          m_response_data.file_name);

  if (m_response_data.file_exist == true)
  {
    // error deleting file
    if (std::remove(&target_file[0]) != 0) throw INTERNAL_SERVER_ERROR_500;
  }
}

// PutMethodHandler
PutMethodHandler::PutMethodHandler(void) {}
PutMethodHandler::PutMethodHandler(const PutMethodHandler& obj)
{
  m_request_data = obj.m_request_data;
  m_response_data = obj.m_response_data;
}
PutMethodHandler::PutMethodHandler(Request& request_data,
                                   Response& response_data)
{
  m_request_data = request_data;
  m_response_data = response_data;
}
PutMethodHandler::~PutMethodHandler(void) {}
PutMethodHandler& PutMethodHandler::operator=(PutMethodHandler const& obj)
{
  if (this != &obj)
  {
    m_request_data = obj.m_request_data;
    m_response_data = obj.m_response_data;
  }
  return (*this);
}
void PutMethodHandler::methodRun()
{
  std::string target_file(m_response_data.file_path +
                          m_response_data.file_name);

  if (m_response_data.file_exist == false)
  {
    std::ofstream new_file_stream(target_file,
                                  std::ios::app | std::ios::binary);
    if (!new_file_stream) throw INTERNAL_SERVER_ERROR_500;
    new_file_stream.write(&m_request_data.body[0], m_request_data.body.size());
    new_file_stream.close();
  }
  else
  {
    std::ofstream append_stream(target_file, std::ios::app);
    std::stringstream content_stream;
    // 파일스트림 실패시 어떤 에러코드를 줄지 모르겠음
    if (!append_stream) throw INTERNAL_SERVER_ERROR_500;
    content_stream.write(&m_request_data.body[0], m_request_data.body.size());
    append_stream << content_stream.str();
    append_stream.close();
  }
}
