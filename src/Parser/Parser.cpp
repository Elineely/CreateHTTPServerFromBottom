#include "Parser.hpp"

#include "Log.hpp"
#include "utils.hpp"

// Constructor
Parser::Parser(void) {}

// Copy Assignment Operator
Parser& Parser::operator=(Parser const& rhs)
{
  if (this != &rhs)
  {
    m_pool = rhs.m_pool;
  }
  return *this;
}

// Destructor
Parser::~Parser(void) {}

// Public member functions

void Parser::saveBufferInPool(char* buf, int recv_size)
{
  if (buf == NULL)
  {
    throw std::invalid_argument("NULL string is not allowed");
  }
  m_pool.line_len += recv_size;
  for (int idx = 0; idx < recv_size; idx += 1)
  {
    m_pool.total_line.push_back(buf[idx]);
  }
}

size_t Parser::findNewline(const char* buf, size_t offset)
{
  if (m_pool.line_len < 2)
  {
    return (0);
  }
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

void Parser::checkBodyType(Request& request)
{
  std::map<std::string, std::string>::iterator it;
  std::map<std::string, std::string>::iterator end_it = request.headers.end();

  if (request.method == "GET" || request.method == "DELETE" ||
      request.method == "HEAD" || request.method == "PATCH")
  {
    request.validation_phase = COMPLETE;
    return;
  }
  it = request.headers.find("content-length");
  if (it != end_it)
  {
    if (it->second == "0")
    {
      request.validation_phase = COMPLETE;
    }
    else
    {
      request.validation_phase = ON_BODY;
    }
    return;
  }
  it = request.headers.find("transfer-encoding");
  if (it != end_it)
  {
    request.validation_phase = ON_CHUNKED_BODY;
    return;
  }
  request.status = BAD_REQUEST_400;
}

// Public member functions
void Parser::readBuffer(char* buf, int recv_size, Request& request)
{
  try
  {
    if (request.validation_phase == COMPLETE)
    {
      return;
    }

    // 클라이언트가 보낸 데이터를 RequestPool 에 저장
    saveBufferInPool(buf, recv_size);

    // Validation 단계에 따라 first-line, header, [body] 를 파싱
    while (request.validation_phase != COMPLETE)
    {
      switch (request.validation_phase)
      {
        case READY:
        {
          parseFirstLine(request);
          break;
        }
        case ON_HEADER:
        {
          parseHeaders(request);
          break;
        }
        case ON_BODY:
        {
          parseBody(request);
          break;
        }
        case ON_CHUNKED_BODY:
        {
          parseChunkedBody(request);
          break;
        }
        default:
          break;
      }
      if (m_pool.line_len == m_pool.offset)
      {
        return;
      }
    }
  }
  catch (std::exception& e)
  {
    // Log::print(ERROR, "Parser readBuffer catches error (%s)", e.what());
  }
}
