#include "Parser.hpp"

#include "Log.hpp"
#include "utils.hpp"
#include <stdlib.h>

void Parser::parseBody(void)
{
  for (size_t idx = m_pool.offset; idx < m_pool.line_len; idx += 1)
  {
    m_request.body.push_back(m_pool.total_line[idx]);
    m_pool.offset += 1;
  }

  long content_length =
    std::atol(m_request.headers["content-length"].c_str());
    // std::strtoll(m_request.headers["content-length"].c_str(), NULL, 10);

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
