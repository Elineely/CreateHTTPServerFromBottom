#ifndef Parser_HPP
#define Parser_HPP

#include <map>
#include <string>
#include <vector>

enum ErrorNum {
  kInvalidMethod;
};

struct RequestPool {
  char* total_line;
  size_t line_len;
  size_t prev_offset;
  size_t offset;
  bool found_newline;
};

struct Request {
  std::string method;
  std::string uri;
  std::string http_version;
  char* first_line;
  std::map<std::string, char*> header;
  std::vector<char> body;
  ErrorNum err_num;
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
  char* ParseFirstLine(void);
  void SaveBufferInPool(char* buf);
  void FindNewlineInPool(void);
};

#endif
