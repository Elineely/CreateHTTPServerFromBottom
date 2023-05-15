#ifndef Parser_HPP
#define Parser_HPP

#include <map>
#include <string>
#include <vector>

enum KeyValuePair {
  KEY = 0,
  VALUE = 1
};

enum StatusCode {
  BAD_REQUEST_400 = 400,
};

enum ValidationStatus {
  READY,
  ON_HEADER,
  ON_BODY,
  COMPLETE
};

struct RequestPool {
  char* total_line;
  size_t line_len;
  size_t prev_offset;
  size_t offset;
};

// TODO: char* 자료형에서 string 으로 변환할 때 '0' 을 null character 로
// 인식하는지 확인 필요
struct Request {
  std::string method;
  std::string uri;
  std::string http_version;
  ValidationStatus validation_phase;
  std::map<std::string, std::string> headers;
  std::vector<char> body;
  StatusCode status;
};

class Parser {
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
  struct Request data_;
  struct RequestPool pool_;

  // Member Functions
  void SaveBufferInPool(char* buf);
  bool FindNewlineInPool(void);
  void ParseFirstLine(void);
  void ParseHeaders(std::map<std::string, std::string>& headers);
  void ParseBody(std::vector<char>& body);
  void ParseChunkedBody(std::vector<char>& body);
};

#endif
