#ifndef REQUEST_HPP
#define REQUEST_HPP

#include <map>
#include <string>
#include <vector>

#include "StatusCode.hpp"
#include "ValidationStatus.hpp"
#include <iostream>
struct Request
{
  ValidationStatus validation_phase;  // 파싱 단계를 알려주는 변수
  StatusCode status;  // 오류 발생 여부를 알려주는 변수
  std::string method;
  std::string uri;
  std::string http_version;
  std::map<std::string, std::string> headers;
  std::vector<char> body;

  // Default Constructor
  Request(void) : validation_phase(READY), status(NO_PROBLEM) {}

  // Destructor
  ~Request(void) {}

  // Copy Constructor
  Request(const Request& src)
  {
    if (this != &src)
    {
      *this = src;
    }
  }

  // Copy Assignment
  Request& operator=(const Request& rhs)
  {

    if (this != &rhs)
    {
      validation_phase = rhs.validation_phase;
      method = rhs.method;
      uri = rhs.uri;
      http_version = rhs.http_version;
      headers = rhs.headers;
      body = rhs.body;
      status = rhs.status;
    }
    return (*this);
  }
};

#endif
