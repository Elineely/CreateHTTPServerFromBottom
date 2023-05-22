#ifndef ResponseGenerator_HPP
#define ResponseGenerator_HPP

#include <string>

class ResponseGenerator
{
 private:
  // uri, root 조합해가지고 타겟이 되는 파일의 경로

  void makeBody();
  void setStartLine();
  void setHeaders();
  void setBody();
  void headerContentType();
  void headerContentLength();
  // void headerConnection();
  // void headerServer();
  // void headerLocation();
  // void headerDate();

 public:
  std::string response_message;
  Response m_response;
  Request m_request;
  Config m_config;

  ResponseGenerator(void);
  ResponseGenerator(Request& request_data, Config& config_data);
  ResponseGenerator(const ResponseGenerator& obj);
  virtual ~ResponseGenerator(void);
  ResponseGenerator& operator=(ResponseGenerator const& obj);
};

#endif