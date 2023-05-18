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

// TODO: HTTP Response 와 공유하기 위해 헤더 파일 이동
enum StatusCode
{
  NO_PROBLEM = 0,
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

  // Default Constructor
  RequestPool(void) : total_line(NULL) {}

  // Destructor
  ~RequestPool(void)
  {
    if (total_line != NULL)
    {
      delete total_line;
    }
  }

  // Copy constructor
  RequestPool(const RequestPool& src)
  {
    if (this != &src)
    {
      *this = src;
    }
  }

  // Copy assignment
  RequestPool& operator=(const RequestPool& rhs)
  {
    if (this != &rhs)
    {
      line_len = rhs.line_len;
      prev_offset = rhs.prev_offset;
      offset = rhs.offset;
      if (total_line != NULL)
      {
        delete total_line;
      }
      total_line = new char[line_len];
      std::memmove(total_line, rhs.total_line, line_len + 1);
    }
    return (*this);
  }
};

// TODO: HTTP Response 와 공유하기 위해 헤더 파일 이동
struct Request
{
  ValidationStatus validation_phase;  // 파싱 단계를 알려주는 변수
  StatusCode status;  // 오류 발생 여부를 알려주는 변수
  std::string method;
  std::string uri;
  std::string http_version;
  std::map<std::string, std::string> headers;
  std::vector<char> body;

  // Default Constructor
  Request(void) : validation_phase(READY), status(NO_PROBLEM) {}

  // Destructor
  ~Request(void) {}

  // Copy Constructor
  Request(const Request& src)
  {
    if (this != &src)
    {
      *this = src;
    }
  }

  // Copy Assignment
  Request& operator=(const Request& rhs)
  {
    if (this != &rhs)
    {
      validation_phase = rhs.validation_phase;
      uri = rhs.uri;
      http_version = rhs.http_version;
      headers = rhs.headers;
      body = rhs.body;
      status = rhs.status;
    }
    return (*this);
  }
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
  void ParseHeaders(void);
  void ParseBody(void);
  void ParseChunkedBody(std::vector<char>& body);
};

#endif
