#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include <map>
#include <string>
#include <vector>

#include "Config.hpp"
#include "Request.hpp"
#include "StatusCode.hpp"

class Response
{
 private:
  std::string m_file_path;
  std::string m_body;
  StatusCode m_status_code;
  Request m_request;
  // config 멤버변수 필요없다면 빼기
  Config m_config;
  bool m_cgi_flag;
  char* m_cgi_bin_path;
  char** m_cgi_params;
  std::string m_response_message;

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
  Response();
  Response(Request& request_data, Config& config_data);
  Response(const Response& obj);
  ~Response();
  Response& operator=(const Response& obj);

  // get fuctions
  std::string get_m_file_path();
  std::string get_m_body();
  StatusCode get_m_status_code();
  Request get_m_request();
  Config get_m_config();
  bool get_m_cgi_flag();
  char* get_m_cgi_bin_path();
  char** get_m_cgi_params();
  std::string get_m_response_message();

  // set functions
  void set_m_file_path(const std::string obj);
  void set_m_body(const std::string obj);
  void set_m_status_code(const StatusCode obj);
  void set_m_request(const Request obj);
  void set_m_config(const Config obj);
  void set_m_cgi_flag(const bool obj);
  void set_m_cgi_bin_path(const char* obj);
  void set_m_cgi_params(const char** obj);
  void set_m_response_message(const std::string obj);

  const char* generateErrorResponseMessage();
  const char* generateResponseMessage();
};

#endif