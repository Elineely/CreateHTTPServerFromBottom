#include "Parser.hpp"

#include "Log.hpp"
#include "utils.hpp"

ssize_t Parser::parseChunkedBodyLength(void)
{
  size_t crlf_idx;

  crlf_idx = findNewline(&m_pool.total_line[0], m_pool.offset);
  if (crlf_idx == m_pool.offset)
  {
    m_request.validation_phase = COMPLETE;
    return (-1);
  }
  else if (crlf_idx == 0)
  {
    return (-1);
  }
  std::string str_chunk_size(&m_pool.total_line[m_pool.offset],
                             crlf_idx - m_pool.offset);
  long long chunk_size = std::strtoll(str_chunk_size.c_str(), NULL, 10);

  m_pool.offset = crlf_idx + 2;
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
    return;
  }

  if (m_pool.line_len - m_pool.offset < chunked_body_size)
  {
    return;
  }

  for (size_t idx = m_pool.offset; idx < m_pool.offset + chunked_body_size;
       ++idx)
  {
    m_request.body.push_back(m_pool.total_line[idx]);
    m_pool.offset += 1;
  }
}
