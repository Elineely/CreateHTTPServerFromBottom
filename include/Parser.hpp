#ifndef PARSER_HPP
#define PARSER_HPP

#include <map>
#include <string>
#include <vector>

#include "Request.hpp"
#include "RequestPool.hpp"

enum KeyValuePair
{
  KEY = 0,
  VALUE = 1
};

class Parser
{
 public:
  // Canonical Form
  Parser(const std::string& max_body_size);
  Parser(const Parser& src);
  virtual ~Parser(void);
  Parser& operator=(Parser const& rhs);

  // Member Functions
  void readBuffer(char* buf);
  ValidationStatus get_validation_phase(void);

 private:
  Parser(void);

  // Member Variables
  struct Request m_data;
  struct RequestPool m_pool;
  size_t m_max_body_size;

  // Member Functions
  void saveBufferInPool(char* buf);
  bool findNewlineInPool(void);
  void parseFirstLine(void);
  void parseHeaders(void);
  void parseBody(void);
  void parseChunkedBody(void);
};

#endif
