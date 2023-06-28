#ifndef RESPONSE_GENERATOR_HPP
#define RESPONSE_GENERATOR_HPP

#include <map>
#include <string>
#include <vector>

#include "Config.hpp"
#include "Log.hpp"
#include "Mime.hpp"
#include "Request.hpp"
#include "Response.hpp"
#include "StatusCode.hpp"

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
  std::vector<char>::iterator m_cgi_body_it;

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
