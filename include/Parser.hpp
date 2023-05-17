#ifndef PARSER_HPP
#define PARSER_HPP

#include <map>
#include <string>
#include <vector>

enum KeyValuePair
{
  KEY = 0,
  VALUE = 1
};

enum StatusCode
{
  BAD_REQUEST_400 = 400,
};

enum ValidationStatus
{
  READY,
  ON_HEADER,
  ON_BODY,
  COMPLETE
};

struct RequestPool
{
  char* total_line;
  size_t line_len;
  size_t prev_offset;
  size_t offset;
};

struct Request
{
  ValidationStatus validation_phase;
  std::string method;
  std::string uri;
  std::string http_version;
  std::map<std::string, std::string> headers;
  std::vector<char> body;
  StatusCode status;
};

class Parser
{
 public:
  // Canonical Form
  Parser(void);
  Parser(const Parser& src);
  virtual ~Parser(void);
  Parser& operator=(Parser const& rhs);

  // Member Functions
  void ReadBuffer(char* buf);

 private:
  // Member Variables
  struct Request m_data;
  struct RequestPool m_pool;

  // Member Functions
  void SaveBufferInPool(char* buf);
  bool FindNewlineInPool(void);
  void ParseFirstLine(void);
  void ParseHeaders(std::map<std::string, std::string>& headers);
  void ParseBody(std::vector<char>& body);
  void ParseChunkedBody(std::vector<char>& body);
};

#endif
