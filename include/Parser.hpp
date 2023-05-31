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
  Parser(const std::string& max_body_size, int client_fd);
  Parser(const Parser& src);
  virtual ~Parser(void);
  Parser& operator=(Parser const& rhs);

  // Member Functions
  void readBuffer(char* buf);
  ValidationStatus get_validation_phase(void);
  int get_m_client_fd(void);

 private:
  Parser(void);

  // Member Variables
  struct Request m_data;
  struct RequestPool m_pool;
  size_t m_max_body_size;
  int m_client_fd;

  // Member Functions
  void saveBufferInPool(char* buf);
  bool findNewlineInPool(void);
  void parseFirstLine(void);
  void parseHeaders(void);
  void parseBody(void);
  void parseChunkedBody(void);
};

#endif
