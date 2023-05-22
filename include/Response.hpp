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

 public:
  Response();
  Response(Request& request_data, Config& config_data);
  Response(const Response& obj);
  ~Response();
  Response& operator=(const Response& obj);

  void makeBody();
  void setStartLine();
  void setHeaders();
  void setBody();
  void generateHeaderContentType();
  void generateHeaderContentLength();
  char* generateErrorResponseMessage();
  char* generateResponseMessage();
};

#endif