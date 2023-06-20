#include <stdlib.h>

#include "Log.hpp"
#include "Parser.hpp"
#include "utils.hpp"

void Parser::parseBody(Request& request)
{
  for (size_t idx = m_pool.offset; idx < m_pool.line_len; idx += 1)
  {
    request.body.push_back(m_pool.total_line[idx]);
    m_pool.offset += 1;
  }

  ssize_t content_length = std::atol(request.headers["content-length"].c_str());

  if (content_length == static_cast<ssize_t>(request.body.size()))
  {
    request.validation_phase = COMPLETE;
  }
  else if (content_length < static_cast<ssize_t>(request.body.size()))
  {
    request.status = BAD_REQUEST_400;
    request.validation_phase = COMPLETE;
  }
}
