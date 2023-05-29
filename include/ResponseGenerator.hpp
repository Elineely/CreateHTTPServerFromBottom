#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include <map>
#include <string>
#include <vector>

#include "Config.hpp"
#include "Mime.hpp"
#include "Request.hpp"
#include "StatusCode.hpp"

struct Response
{
  bool auto_index;
  std::string file_name;
  std::string file_path;
  bool file_exist;
  bool path_exist;
  std::vector<char> body;
  std::vector<char> response_message;
  bool cgi_flag;
  char* cgi_bin_path;
  int pipe_fd;
};

class ResponseGenerator
{
 private:
  Request m_request;
  Response m_response;
  std::string m_target_file;

  void appendStrToResponse_message(std::string str);
  void appendStrToBody(std::string str);
  std::string statusCodeToString();

  // for genearte Start Line
  void generateVersion();
  void generateStatusCode();
  void generateReasonPhrase();
  // for generate Headers
  void generateContentType();
  void generateContentLength();
  // for generate error body
  void generateErrorBody();
  // for generate Response Message
  void setStartLine();
  void setHeaders();
  void setBody();
  void setErrorBody();

 public:
  ResponseGenerator();
  ResponseGenerator(Request& request_data, Response& response_data);
  ResponseGenerator(const ResponseGenerator& obj);
  ~ResponseGenerator();
  ResponseGenerator& operator=(const ResponseGenerator& obj);

  const char* generateErrorResponseMessage();
  const char* generateResponseMessage();
};

#endif
