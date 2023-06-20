#include "Log.hpp"
#include "Parser.hpp"
#include "utils.hpp"

void Parser::parseFirstLine(Request& request)
{
  size_t crlf_idx;

  crlf_idx = findNewline(&m_pool.total_line[0], m_pool.offset);
  if (crlf_idx == 0)
  {
    throw std::invalid_argument("Need more buffer in parseFirstLine");
  }
  m_pool.offset = crlf_idx + 2;

  std::string input(&m_pool.total_line[0],
                    crlf_idx);  // \r\n 은 제외하고 input 에 저장
  std::string method;
  std::string uri;
  std::string http_version;
  size_t idx1;
  size_t idx2;

  // 1. HTTP Method 탐색
  idx1 = input.find_first_of(' ', 0);
  method = input.substr(0, idx1);
  if (method != "GET" && method != "POST" && method != "DELETE" &&
      method != "HEAD" && method != "PUT")
  {
    request.status = METHOD_NOT_ALLOWED_405;
  }

  // 2. URI 탐색
  idx2 = input.find_first_of(' ', idx1 + 1);
  // EXCEPTION: Method 뒤에 아무 정보도 없는 경우
  if (idx2 == std::string::npos)
  {
    request.status = BAD_REQUEST_400;
    throw std::invalid_argument("There should be a whitespace after method");
  }
  uri = input.substr(idx1 + 1, idx2 - idx1 - 1);
  if (uri.at(0) != '/')
  {
    request.status = BAD_REQUEST_400;
    throw std::invalid_argument("URI should start with slash(/)");
  }

  // 3. HTTP Version 탐색
  http_version =
      input.substr(idx2 + 1, input.find_first_of('\r', idx2 + 1) - idx2 - 1);
  if (http_version != "HTTP/1.0" && http_version != "HTTP/1.1")
  {
    request.status = BAD_REQUEST_400;
    throw std::invalid_argument("HTTP version should be either 1.0 or 1.1");
  }

  request.method = method;
  request.uri = uri;
  request.http_version = http_version;
  request.validation_phase = ON_HEADER;
}
