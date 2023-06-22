#include "Log.hpp"
#include "Parser.hpp"
#include "utils.hpp"

void Parser::parseChunkedBody(Request& request)
{
  size_t crlf_idx1 = 0;
  size_t crlf_idx2 = 0;

  crlf_idx1 = findNewline(&m_pool.total_line[0], m_pool.offset);
  if (crlf_idx1 == 0)
  {
    throw(std::invalid_argument("need more buffer"));
  }
  crlf_idx2 = findNewline(&m_pool.total_line[0], crlf_idx1 + 2);
  if (crlf_idx2 == 0)
  {
    throw(std::invalid_argument("need more buffer"));
  }

  if (crlf_idx1 == crlf_idx2 - 2)
  {
    request.validation_phase = COMPLETE;
    return;
  }

  std::string str_chunk_size(&m_pool.total_line[m_pool.offset],
                             crlf_idx1 - m_pool.offset);
  // long chunk_size;

  // chunk_size = std::atol(str_chunk_size.c_str());
  for (size_t idx = crlf_idx1 + 2; idx < crlf_idx2; ++idx)
  {
    request.body.push_back(m_pool.total_line[idx]);
  }
  m_pool.offset = crlf_idx2 + 2;
}
