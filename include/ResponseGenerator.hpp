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
  std::string index_name;
  bool index_exist;
  std::string file_path;
  bool path_exist;
  std::string root_path;
  std::vector<char> body;
  StatusCode status_code;
  bool cgi_flag;
  bool is_cgi_timeout;
  std::string cgi_bin_path;
  std::string uploaded_path;
  std::vector<char> response_message;
  std::vector<char> error_page_vector;
  int static_read_file_fd;
  off_t static_read_file_size;
  int static_write_file_fd;
  int read_pipe_fd;
  int write_pipe_fd;
  int cgi_child_pid;
  bool error_keyword;

  Response();
  Response(const Response& obj);
  ~Response();
  Response& operator=(const Response& obj);
};

class ResponseGenerator
{
 private:
  Request& m_request;
  Response& m_response;
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

  std::vector<char> generateErrorResponseMessage();

 public:
  ResponseGenerator(const ResponseGenerator& obj);
  ResponseGenerator(Request& request_data, Response& response_data);
  ~ResponseGenerator();
  ResponseGenerator& operator=(const ResponseGenerator& obj);

  std::vector<char>& generateResponseMessage();
};

#endif
