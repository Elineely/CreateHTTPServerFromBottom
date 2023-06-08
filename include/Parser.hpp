#ifndef NEW_PARSER_HPP
#define NEW_PARSER_HPP

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
  Parser(const std::string& max_body_size);
  Parser(const Parser& src);
  virtual ~Parser(void);
  Parser& operator=(Parser const& rhs);

  // Member Functions
  void readBuffer(char* buf, int recv_size);
  ValidationStatus get_validation_phase(void);
  struct Request& get_request(void);
    
 private:

  // Member Variables
  struct Request m_request;
  struct RequestPool m_pool;
  size_t m_max_body_size;

  // Member Functions
  void saveBufferInPool(char* buf, int recv_size);
  size_t findNewline(const char* buf, size_t offset);
  void checkBodyType(void);
  void parseFirstLine(void);
  void parseHeaders(void);
  void parseBody(void);
  ssize_t parseChunkedBodyLength(void);
  void parseChunkedBody(ssize_t chunked_body_size);

};

#endif
