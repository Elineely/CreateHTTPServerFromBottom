#include "Parser.hpp"

#include "Log.hpp"
#include "utils.hpp"

void Parser::parseHeaders(Request& request)
{
  size_t crlf_idx;

  crlf_idx = findNewline(&m_pool.total_line[0], m_pool.offset);
  if (crlf_idx == m_pool.offset)
  {
    request.validation_phase = ON_BODY;
    checkBodyType(request);
    m_pool.offset = m_pool.offset + 2;
    return;
  }
  else if (crlf_idx == 0)
  {
    throw std::invalid_argument("Need more buffer in parseHeaders");
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
    request.status = BAD_REQUEST_400;
    throw std::invalid_argument("[HEADERS] There is no key.");
  }

  key = ft_toLower(input.substr(0, idx1));
  if (request.headers.find(key) != request.headers.end())
  {
    request.status = BAD_REQUEST_400;
    throw std::invalid_argument("HTTP header field should not be duplicated");
  }

  value = ft_strtrim(input.substr(idx1 + 1, (crlf_idx - idx1 - 1)));
  request.headers[key] = value;
}
