#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include <map>
#include <string>
#include <vector>

#include "./Config.hpp"
#include "./Request.hpp"
#include "./StatusCode.hpp"

struct Response
{
  std::string accepted_method;
  bool auto_index;
  std::string file_name;
  bool file_exist;
  std::string file_path;
  bool path_exist;
  std::string save_path;
  std::vector<char> body;
  StatusCode status_code;
  bool cgi_flag;
  std::string cgi_bin_path;
  std::string uploaded_path;
  // char** cgi_params;
  // 리스폰스 메세지를 vector로 해도 괜찮을까?
  std::vector<char> response_message;
  int pipe_fd;
};

class ResponseGenerator
{
 private:
  Request m_request;
  Response m_response;

  // for genearte Start Line
  void generateVersion();
  void generateStatusCode();
  void generateReasonPhrase();
  // for generate Headers
  void generateContentType();
  void generateContentLength();
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