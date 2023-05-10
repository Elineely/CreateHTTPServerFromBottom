#ifndef Parser_HPP
#define Parser_HPP

#include <map>
#include <string>
#include <vector>

enum StatusCode {
  BAD_REQUEST_400 = 400,
};

enum ValidationStatus {
  NOT_INSPECTED = -1,
  VALID,
  INVALID
};

struct RequestPool {
  char* total_line;
  size_t line_len;
  size_t prev_offset;
  size_t offset;
  bool found_newline;
};

// TODO: char* 자료형에서 string 으로 변환할 때 '0' 을 null character 로
// 인식하는지 확인 필요
struct Request {
  std::string method;
  std::string uri;
  std::string http_version;
  ValidationStatus first_line_validation;
  std::map<std::string, char*> header;
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
  void ParseFirstLine(void);
  void SaveBufferInPool(char* buf);
  void FindNewlineInPool(void);
};

#endif
