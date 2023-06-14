#include "Parser.hpp"

#include <cstring>
#include <iostream>
#include <sstream>

#include "Log.hpp"
#include "utils.hpp"

// Default Constructor
Parser::Parser(void) {}

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
  }
  return *this;
}

// Public member functions
struct Request& Parser::get_request(void) { return m_request; }

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
  // LOG_DEBUG("recv_size: %d", recv_size);
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

void Parser::checkBodyType(void)
{
  std::map<std::string, std::string>::iterator it;
  std::map<std::string, std::string>::iterator end_it = m_request.headers.end();

  if (m_request.method == "GET" || m_request.method == "DELETE" ||
      m_request.method == "HEAD" || m_request.method == "PATCH")
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

    // LOG_DEBUG("recv_size: %d", recv_size);

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
          parseChunkedBody();
          break;
        }
        default:
          break;
      }

      if (m_request.validation_phase == COMPLETE)
      {
        return;
      }
    }
  }
  catch (std::exception& e)
  {
    // LOG_ERROR("Parser readBuffer catches error (%s)", e.what());
  }
}
