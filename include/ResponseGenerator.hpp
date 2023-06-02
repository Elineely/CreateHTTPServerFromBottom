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
  std::string accepted_method;
  bool redirection_exist;
  std::string rediretion_location;
  bool auto_index;
  std::string file_name;
  bool file_exist;
  std::string file_path;
  bool path_exist;
  std::vector<char> body;
  StatusCode status_code;
  bool cgi_flag;
  std::string cgi_bin_path;
  std::string uploaded_path;
  std::vector<char> response_message;
  int read_pipe_fd;
  int cgi_child_pid;

  Response();
  Response(const Response& obj);
  ~Response();
  Response& operator=(const Response& obj);
};


class ResponseGenerator
{
 private:
  Request m_request;
  Response m_response;
  // Request& m_request;
  // Response& m_response;
  Mime mime;
  StatusStr status_str;
  std::string m_target_file;

  std::string m_cgi_content_type;
  std::vector<char> m_cgi_body;

  void appendStrToResponse_message(std::string str);
  void appendStrToBody(std::string str);
  std::string statusCodeToString();

  void cgiDataProcess();

  // for genearte Start Line
  void generateVersion();
  void generateStatusCode();
  void generateReasonPhrase();
  // for generate Headers
  void generateContentType();
  void generateContentLength();
  void generateServer();
  void generateLocation();
  void generateDate();
  // for generate error body
  void generateErrorBody();
  // for generate Response Message
  void setStartLine();
  void setHeaders();
  void setBody();

  ResponseGenerator();
 public:
  ResponseGenerator(const ResponseGenerator& obj);
  ResponseGenerator(Request& request_data, Response& response_data);
  ~ResponseGenerator();
  ResponseGenerator& operator=(const ResponseGenerator& obj);

  std::vector<char> generateErrorResponseMessage();
  std::vector<char> generateResponseMessage();
};

#endif
