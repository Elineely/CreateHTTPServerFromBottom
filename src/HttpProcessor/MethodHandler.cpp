#include "MethodHandler.hpp"

// Abstract MethodHandler
MethodHandler::MethodHandler(void) {}

MethodHandler::MethodHandler(const MethodHandler& obj)
{
  this->m_config_data = obj.m_config_data;
  this->m_request_data = obj.m_request_data;
  this->m_response_data = obj.m_response_data;
}

MethodHandler::MethodHandler(const Request& request_data,
                             const Config& config_data)
{
  m_request_data = request_data;
  // m_config_data = config_data;
}

MethodHandler& MethodHandler::operator=(MethodHandler const& obj)
{
  if (this != &obj)
  {
    // this->m_config_data = obj.m_config_data;
    this->m_request_data = obj.m_request_data;
    this->m_response_data = obj.m_response_data;
  }
  return (*this);
}

MethodHandler::~MethodHandler(void) {}

// GET
GetMethodHandler::GetMethodHandler(void) {}

GetMethodHandler::GetMethodHandler(const GetMethodHandler& obj)
{
  // m_config_data = obj.m_config_data;
  m_request_data = obj.m_request_data;
  m_response_data = obj.m_response_data;
}
GetMethodHandler::GetMethodHandler(const Request& request_data,
                                   const Config& config_data)
{
  m_request_data = request_data;
  m_config_data = config_data;
}

GetMethodHandler::~GetMethodHandler(void) {}

GetMethodHandler& GetMethodHandler::operator=(GetMethodHandler const& obj)
{
  if (this != &obj)
  {
    this->m_config_data = obj.m_config_data;
    this->m_request_data = obj.m_request_data;
    this->m_response_data = obj.m_response_data;
  }
  return (*this);
}

void GetMethodHandler::makeBody() {}

// // 추후 수정 필요하면 하기
// int openFile(std::string root, std::string uri)
// {
//   int fd;
//   // parser config char*로 받기?
//   fd = open("/Users/hogkim/webserv/src/Handler/index.html",
//             O_RDONLY);  //? 쓸수 있나??
//   if (fd < 0) throw 404;
//   return (fd);
// }
