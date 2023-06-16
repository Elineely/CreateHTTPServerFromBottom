#ifndef PARSER_HPP
#define PARSER_HPP

#include <map>
#include <string>
#include <vector>

#include "Request.hpp"
#include "RequestPool.hpp"

#define MB_TO_BYTE 1048576

enum KeyValuePair
{
  KEY = 0,
  VALUE = 1
};

class Parser
{
 public:
  // Canonical Form
  Parser(void);
  Parser(const Parser& src);
  ~Parser(void);
  Parser& operator=(Parser const& rhs);

  // Member Functions
  void readBuffer(char* buf, int recv_size, Request& request);

 private:
  // Member Variables
  struct RequestPool m_pool;

  // Member Functions
  void saveBufferInPool(char* buf, int recv_size);
  size_t findNewline(const char* buf, size_t offset);
  void checkBodyType(Request& request);
  void parseFirstLine(Request& request);
  void parseHeaders(Request& request);
  void parseBody(Request& request);
  void parseChunkedBody(Request& request);
};

#endif
