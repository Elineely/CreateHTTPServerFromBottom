#include "Parser.hpp"

#include <stdio.h>
#include <unistd.h>

#include <cstring>
#include <iostream>
#include <sstream>
#include <string>

#include "Log.hpp"
#include "utils.hpp"

// Default Constructor
Parser::Parser(void) : m_max_body_size(0) {}

// Constructor
Parser::Parser(const std::string& max_body_size)
{
  std::istringstream iss(max_body_size);
  size_t value;

  iss >> value;
  m_max_body_size = value * MB_TO_BYTE;  // Binary 기준으로 변환
  LOG_DEBUG("m_max_body_size: %d", m_max_body_size);
}

// Destructor
Parser::~Parser(void) {}

// Copy Constructor
Parser::Parser(const Parser& src)
{
  if (this != &src)
  {
    *this = src;
  }
}

// Copy Assignment Operator
Parser& Parser::operator=(Parser const& rhs)
{
  if (this != &rhs)
  {
    m_request = rhs.m_request;
    m_pool = rhs.m_pool;
    m_max_body_size = rhs.m_max_body_size;
  }
  return *this;
}

// Public member functions
struct Request& Parser::get_request(void)
{
  return m_request;
}

ValidationStatus Parser::get_validation_phase(void)
{
  return (m_request.validation_phase);
}

void Parser::saveBufferInPool(char* buf, int recv_size)
{
  if (buf == NULL)
  {
    throw std::invalid_argument("NULL string is not allowed");
  }
  LOG_DEBUG("recv_size: %d", recv_size);
  m_pool.line_len += recv_size;
  for (int idx = 0; idx < recv_size; idx += 1)
  {
    m_pool.total_line.push_back(buf[idx]);
  }
}

size_t Parser::findNewline(const char* buf, size_t offset)
{
  for (size_t idx = offset; idx <= m_pool.line_len - 2; idx += 1)
  {
    if (buf[idx] != '\r')
    {
      continue;
    }
    if (std::strncmp(&buf[idx], "\r\n", 2) == 0)
    {
      return (idx);
    }
  }
  return (0);
}

void Parser::parseFirstLine(void)
{
  LOG_DEBUG("parse_FIrstLine");
  size_t crlf_idx;

  crlf_idx = findNewline(&m_pool.total_line[0], m_pool.offset);
  if (crlf_idx == 0)
  {
    return ;
  }
  m_pool.offset = crlf_idx + 2;
  
  std::string input(&m_pool.total_line[0], crlf_idx); // \r\n 은 제외하고 input 에 저장
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
    m_request.status = BAD_REQUEST_400;
    throw std::invalid_argument("Method is not acceptable");
  }

  // 2. URI 탐색
  idx2 = input.find_first_of(' ', idx1 + 1);
  // EXCEPTION: Method 뒤에 아무 정보도 없는 경우
  if (idx2 == std::string::npos)
  {
    m_request.status = BAD_REQUEST_400;
    throw std::invalid_argument("There should be a whitespace after method");
  }
  uri = input.substr(idx1 + 1, idx2 - idx1 - 1);
  if (uri.at(0) != '/')
  {
    m_request.status = BAD_REQUEST_400;
    throw std::invalid_argument("URI should start with slash(/)");
  }

  // 3. HTTP Version 탐색
  http_version =
      input.substr(idx2 + 1, input.find_first_of('\r', idx2 + 1) - idx2 - 1);
  if (http_version != "HTTP/1.0" && http_version != "HTTP/1.1")
  {
    m_request.status = BAD_REQUEST_400;
    throw std::invalid_argument("HTTP version should be either 1.0 or 1.1");
  }

  m_request.method = method;
  m_request.uri = uri;
  m_request.http_version = http_version;
  m_request.validation_phase = ON_HEADER;
  LOG_DEBUG("request method: %s", method.c_str());
  LOG_DEBUG("request uri: %s", uri.c_str());
}

void Parser::checkBodyType(void)
{
  std::map<std::string, std::string>::iterator it;
  std::map<std::string, std::string>::iterator end_it = m_request.headers.end();

  if (m_request.method == "GET" || m_request.method == "DELETE")
  {
    m_request.validation_phase = COMPLETE;
    return;
  }
  it = m_request.headers.find("content-length");
  if (it != end_it)
  {
    m_request.validation_phase = ON_BODY;
    return;
  }
  it = m_request.headers.find("transfer-encoding");
  if (it != end_it)
  {
    m_request.validation_phase = ON_CHUNKED_BODY;
    return;
  }
  m_request.status = BAD_REQUEST_400;
}

void Parser::parseBody(void)
{
  for (size_t idx = m_pool.offset; idx < m_pool.line_len; idx += 1)
  {
    m_request.body.push_back(m_pool.total_line[idx]);
    m_pool.offset += 1;
  }

  long long content_length =
    std::strtoll(m_request.headers["content-length"].c_str(), NULL, 10);
  
  if (content_length == m_request.body.size())
  {
    m_request.validation_phase = COMPLETE;
  }
  else if (content_length < m_request.body.size())
  {
    m_request.status = BAD_REQUEST_400;
    m_request.validation_phase = COMPLETE;
  }
  LOG_DEBUG("m_pool.line_len: %d, m_pool.offset: %d, content-length: %d, body_size: %d", m_pool.line_len, m_pool.offset, content_length, m_request.body.size());
}

ssize_t Parser::parseChunkedBodyLength(void)
{
  size_t crlf_idx;

  crlf_idx = findNewline(&m_pool.total_line[0], m_pool.offset);
  if (crlf_idx == m_pool.offset)
  {
    m_request.validation_phase = COMPLETE;
    return (-1);
  }
  std::string str_chunk_size(&m_pool.total_line[m_pool.offset], crlf_idx);
  long long chunk_size = std::strtoll(str_chunk_size.c_str(), NULL, 10);

  m_pool.offset += 2;
  if (chunk_size < 0)
  {
    m_request.status = BAD_REQUEST_400;
    return (-1);
  }
  return (static_cast<ssize_t>(chunk_size));
}

void Parser::parseChunkedBody(ssize_t chunked_body_size)
{
  if (chunked_body_size == -1)
  {
    return ;
  }

  /*
  
  5\r\n
  hello\r\n
  0\r\n\r\n

  */

  if (m_pool.line_len - m_pool.offset < chunked_body_size)
  {
    return ;
  }
  size_t crlf_idx;

  crlf_idx = findNewline(&m_pool.total_line[0], m_pool.offset);
  if (crlf_idx == 0)
  {
    return ;
  }
  for (size_t idx = m_pool.offset; idx < crlf_idx; ++idx)
  {
    m_request.body.push_back(m_pool.total_line[idx]);
    m_pool.offset += 1;
  }
  
}


void Parser::parseHeaders(void)
{
  size_t crlf_idx;

  crlf_idx = findNewline(&m_pool.total_line[0], m_pool.offset);
  if (crlf_idx == m_pool.offset)
  {
    m_request.validation_phase = ON_BODY;
    checkBodyType();
    m_pool.offset = m_pool.offset + 2;
    return ; 
  }
  // \r\n 은 제외하고 input 에 저장
  std::string input(&m_pool.total_line[m_pool.offset], crlf_idx - m_pool.offset);
  std::string key;
  std::string value;
  size_t idx1;
  m_pool.offset = crlf_idx + 2;

  idx1 = input.find_first_of(':', 0);
  if (idx1 == std::string::npos)
  {
    m_request.status = BAD_REQUEST_400;
    throw std::invalid_argument("[HEADERS] There is no key.");
  }

  key = ft_toLower(input.substr(0, idx1));
  if (m_request.headers.find(key) != m_request.headers.end())
  {
    m_request.status = BAD_REQUEST_400;
    throw std::invalid_argument("HTTP header field should not be duplicated");
  }

  value = ft_strtrim(input.substr(idx1 + 1, (crlf_idx - idx1 - 1)));
  LOG_DEBUG("key: %s, value: %s", key.c_str(), value.c_str());
  m_request.headers[key] = value;
}

// Public member functions
void Parser::readBuffer(char* buf, int recv_size)
{
  try
  {
    // std::cout << buf << std::endl;
    if (m_request.validation_phase == COMPLETE)
    {
      return;
    }

    LOG_DEBUG("recv_size: %d", recv_size);

    // 클라이언트가 보낸 데이터를 RequestPool 에 저장
    saveBufferInPool(buf, recv_size);

    // Validation 단계에 따라 first-line, header, [body] 를 파싱
    while (m_request.validation_phase != COMPLETE)
    {
      switch (m_request.validation_phase)
      {
        case READY:
        {
          parseFirstLine();
          break;
        }
        case ON_HEADER:
        {
          parseHeaders();
          break;
        }
        case ON_BODY:
        {
          parseBody();
          break;
        }
        case ON_CHUNKED_BODY:
        {
          ssize_t chunked_body_size;

          chunked_body_size = parseChunkedBodyLength();
          parseChunkedBody(chunked_body_size);
          break;
        }
        default:
          break;
      }

      if (m_request.validation_phase == COMPLETE || m_pool.line_len == m_pool.offset)
      {
        return;
      }
    }
  }
  catch (std::exception& e)
  {
    LOG_ERROR("Parser readBuffer catches error (%s)", e.what());
  }
}
