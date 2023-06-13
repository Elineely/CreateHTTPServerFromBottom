#include "CgiHandler.hpp"

#include <fcntl.h>

#include "Log.hpp"

#define SUCCESS 0
#define RETURN_ERROR -1

#define READ 0
#define WRITE 1

#define CHILD_PROCESS 0

// TODO : 제출 전 수정
// #define SERVER_NAME std::string("10.12.9.2")
// #define SERVER_PORT std::string("80")

/* ************************ */
/* CgiHandler virtual class */
/* ************************ */
// no need to make canonical form for virtual class?
CgiHandler::CgiHandler() {}

CgiHandler::CgiHandler(Request& requset_data, Response& response_data)
    : m_request_data(requset_data), m_response_data(response_data)
{
}

CgiHandler::~CgiHandler() {}

Response CgiHandler::get_m_response_data() { return (m_response_data); }

const char* CgiHandler::PipeForkException::what() const throw()
{
  return ("pipe() or fork() system function error");
}

const char* CgiHandler::ExecutionException::what() const throw()
{
  return ("function error");
}

const char* CgiHandler::KqueueException::what() const throw()
{
  return ("something wrong with kqueue()");
}
