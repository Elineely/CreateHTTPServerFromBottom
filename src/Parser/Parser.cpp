#include "Parser.hpp"

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
    m_data = rhs.m_data;
    if (m_pool.total_line != NULL)
    {
      delete m_pool.total_line;
    }
    m_pool = rhs.m_pool;
  }
  return *this;
}

// Public member functions
ValidationStatus Parser::get_validation_phase(void)
{
  return (m_data.validation_phase);
}

struct Request& Parser::get_request(void) { return (m_data); }

// Private member functions
void Parser::parseFirstLine(void)
{
  // \r\n 은 제외하고 input 에 저장
  std::string input(m_pool.total_line, 0,
                    m_pool.offset - m_pool.prev_offset - 2);
  std::string method;
  std::string uri;
  std::string http_version;
  size_t idx1;
  size_t idx2;

  // 1. HTTP Method 탐색
  idx1 = input.find_first_of(' ', 0);
  method = input.substr(0, idx1);
  if (method != "GET" && method != "POST" && method != "DELETE")
  {
    m_data.status = BAD_REQUEST_400;
    throw std::invalid_argument("Method is not acceptable");
  }

  // 2. URI 탐색
  idx2 = input.find_first_of(' ', idx1 + 1);
  // EXCEPTION: Method 뒤에 아무 정보도 없는 경우
  if (idx2 == std::string::npos)
  {
    m_data.status = BAD_REQUEST_400;
    throw std::invalid_argument("There should be a whitespace after method");
  }
  uri = input.substr(idx1 + 1, idx2 - idx1 - 1);
  if (uri.at(0) != '/')
  {
    m_data.status = BAD_REQUEST_400;
    throw std::invalid_argument("URI should start with slash(/)");
  }

  // 3. HTTP Version 탐색
  // TODO: HTTP 1.0 버전도 호환 가능하게 받을 것인지?
  http_version =
      input.substr(idx2 + 1, input.find_first_of('\r', idx2 + 1) - idx2 - 1);
  if (http_version != "HTTP/1.0" && http_version != "HTTP/1.1")
  {
    m_data.status = BAD_REQUEST_400;
    throw std::invalid_argument("HTTP version should be either 1.0 or 1.1");
  }

  m_data.method = method;
  m_data.uri = uri;
  m_data.http_version = http_version;
  m_data.validation_phase = ON_HEADER;
  Log::debug("request method: %s", method.c_str());
  Log::debug("request uri: %s", uri.c_str());
}

// TODO: vector 의 push_back 처럼 size 를 2배씩 늘려야 효율이 좋을까?
void Parser::saveBufferInPool(char* buf)
{
  size_t buf_len;
  char* temp_buf;

  if (buf == NULL)
  {
    throw std::invalid_argument("NULL string is not allowed");
  }

  buf_len = std::strlen(buf);
  m_pool.line_len += buf_len;
  if (m_pool.total_line == NULL)
  {
    m_pool.total_line = new char[buf_len + 1]();
    std::memmove(m_pool.total_line, buf, buf_len + 1);
  }
  else
  {
    temp_buf = ft_strdup(m_pool.total_line);
    delete m_pool.total_line;
    // TODO: strjoin 보다 효율적인 문자열 병합 방법이 있는지 찾기
    m_pool.total_line = ft_strjoin(temp_buf, buf);
    delete temp_buf;
  }
}

bool Parser::findNewlineInPool(void)
{
  const char* find;
  const char* target;

  find = std::strstr(m_pool.total_line + m_pool.offset, "\r\n");
  if (find == NULL)
  {
    return (false);
  }

  m_pool.prev_offset = m_pool.offset;
  if (m_pool.prev_offset < 2)
  {
    target = m_pool.total_line;
  }
  else
  {
    target = m_pool.total_line + m_pool.prev_offset - 2;
  }
  if (std::strncmp(target, "\r\n\r\n", 4) == 0)
  {
    if (m_data.method == "POST" &&
        (m_data.headers.find("content-length") != m_data.headers.end() ||
         m_data.headers["transfer-encoding"] == "chunked"))
    {
      m_data.validation_phase = ON_BODY;
    }
    else
    {
      m_data.validation_phase = COMPLETE;
    }
    m_pool.offset = m_pool.prev_offset + 2;
  }
  else
  {
    m_pool.offset = static_cast<size_t>((find + 2) - m_pool.total_line);
  }
  return (true);
}

void Parser::parseHeaders(void)
{
  do
  {
    // '\r\n' 을 포함해서 저장
    std::string input(m_pool.total_line, m_pool.prev_offset,
                      m_pool.offset - m_pool.prev_offset);
    std::string key;
    std::string value;
    size_t idx1;
    size_t idx2;

    idx1 = input.find_first_of(':', 0);
    if (idx1 == std::string::npos)
    {
      m_data.status = BAD_REQUEST_400;
      throw std::invalid_argument("[HEADERS] There is no key.");
    }

    key = ft_toLower(input.substr(0, idx1));
    if (m_data.headers.find(key) != m_data.headers.end())
    {
      m_data.status = BAD_REQUEST_400;
      throw std::invalid_argument("HTTP header field should not be duplicated");
    }

    idx2 = input.find_first_of('\n', idx1 + 1);
    if (idx2 == std::string::npos)
    {
      m_data.status = BAD_REQUEST_400;
      throw std::invalid_argument("[HEADERS] There is no CRLF.");
    }

    value = ft_strtrim(input.substr(idx1 + 1, (idx2 - idx1 - 1)));
    m_data.headers[key] = value;
  } while (findNewlineInPool() == true && m_data.validation_phase == ON_HEADER);
}

void Parser::parseBody(void)
{
  long long content_length =
      std::strtoll(m_data.headers["content-length"].c_str(), NULL, 10);

  if (content_length < 0)
  {
    m_data.status = BAD_REQUEST_400;
    throw std::invalid_argument("Content-length should be positive value");
  }

  m_data.body.reserve(m_pool.line_len - m_pool.offset);
  for (size_t idx = m_pool.offset; idx < m_pool.line_len; idx += 1)
  {
    m_data.body.push_back(*(m_pool.total_line + idx));
  }

  if (static_cast<size_t>(content_length) != m_data.body.size())
  {
    m_data.status = BAD_REQUEST_400;
    throw std::invalid_argument(
        "Content-length and body length should be equal");
  }

  m_data.validation_phase = COMPLETE;
}

void Parser::parseChunkedBody(void)
{
  while (true)
  {
    char* body_curr =
        m_pool.total_line + m_pool.prev_offset;  // 마지막 파싱 위치
    char* find = std::strstr(body_curr, "\r\n");
    if (find == NULL)
    {
      break;
    }

    std::string str_chunk_size(body_curr,
                               static_cast<size_t>(find - body_curr));
    long long chunk_size = std::stoll(str_chunk_size);
    if (str_chunk_size != "0" && chunk_size <= 0)
    {
      m_data.status = BAD_REQUEST_400;
      throw std::invalid_argument("Chunk size should be positive value");
    }

    char* next_newline = std::strstr(find + 2, "\r\n");
    if (next_newline == NULL)
    {
      break;
    }

    if (static_cast<long long>(next_newline - (find + 2)) != chunk_size)
    {
      m_data.status = BAD_REQUEST_400;
      throw std::invalid_argument("Chunk body should be equal with chunk size");
    }

    for (char* ptr = find + 2; ptr < next_newline; ptr += 1)
    {
      m_data.body.push_back(*ptr);
    }

    m_pool.prev_offset = next_newline - m_pool.total_line + 2;
    m_pool.offset = m_pool.prev_offset;
  }
}

// Public member functions
void Parser::readBuffer(char* buf)
{
  try
  {
    if (m_data.validation_phase == COMPLETE)
    {
      return;
    }

    // 클라이언트가 보낸 데이터를 RequestPool 에 저장
    saveBufferInPool(buf);

    // 마지막으로 CRLF 를 찾은 지점 이후에 CRLF 가 들어왔는지 확인
    if (findNewlineInPool() == false && m_data.validation_phase != ON_BODY)
    {
      return;
    }

    // Validation 단계에 따라 first-line, header, [body] 를 파싱
    while (m_data.validation_phase != COMPLETE)
    {
      switch (m_data.validation_phase)
      {
        case READY:
          parseFirstLine();
          break;
        case ON_HEADER:
          parseHeaders();
          break;
        case ON_BODY:
          if (m_data.headers.find("content-length") != m_data.headers.end())
          {
            parseBody();
          }
          else if (m_data.headers["transfer-encoding"] == "chunked")
          {
            parseChunkedBody();
          }
          if (m_data.body.size() > m_max_body_size)
          {
            m_data.status = BAD_REQUEST_400;
            throw std::invalid_argument("Exceed max body size.");
          }
        default:
          break;
      }

      if (findNewlineInPool() == false && m_data.validation_phase == COMPLETE)
      {
        break;
      }
    }
  }
  catch (std::exception& e)
  {
    Log::error("Parser readBuffer catches error (%s)", e.what());
  }
}