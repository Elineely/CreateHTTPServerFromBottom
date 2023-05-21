#include "MethodHandler.hpp"

// Abstract MethodHandler
MethodHandler::MethodHandler(void) {}

MethodHandler::MethodHandler(const MethodHandler& obj)
{
  // this->m_config_data = obj.m_config_data;
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

Response MethodHandler::get_m_response_data(void) { return (m_response_data); }

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

// 중요!
void GetMethodHandler::generateResponse(void)
{
  m_response_data.body = createBody("./index.html");

  // config -> autoindex 확인 -> on 일시 목록 형식으로 가져오기 -
  // 현재 루트 폴더 내에 있는 파일 이름 읽는다? ->확인 할것
  // minishell cd, ls

  // autoindex off 일시 root 폴더의 index 순서대로 읽어오기(index.php
  // index.html 등등)

  // 정적 페이지 보는 기능 구현
  //  config root 경로 + index[0] ~index[n]; -> 도중에 열리는 파일이 있으면
  //  멈춤. m_config_data.getRootDir();으로 경로를 char*형으로 받을 예정
  // minishell cd
  // int fd;
  // char buffer[25];
  // fd = openFile("test", "index.html");
  // // 읽은 글자수 반환 ->? read() file ->eof?? ? response->body;
  // read(fd, buffer, 25);
  // for (int i = 0; i < 25; ++i)
  // {
  //   // std::cout << buffer[i] << std::endl;
  //   this->m_response_data.body.push_back(buffer[i]);
  // }
  // for (int i = 0; i < 25; ++i)
  // {
  //   std::cout << m_response_data.body[i];
  // }
  // std::cout << std::endl;
  // if (is_file_needs_to_be_created) {
  //   _fd = ::open(_path.c_str(), O_CREAT | O_RDWR | O_TRUNC, 00755);
  // } else {
  // _fd = open(_path.c_str(), O_RDONLY);
  // }
  // return _fd >= 0;
}
