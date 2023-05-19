#ifndef HttpProcessor_HPP
#define HttpProcessor_HPP

class HttpProcessor
{
 public:
  HttpProcessor(void);
  HttpProcessor(Request& request_data, Config& config_data);
  HttpProcessor(const HttpProcessor& obj);
  virtual ~HttpProcessor(void);
  HttpProcessor& operator=(HttpProcessor const& obj);

  // Member Functions
  char* generateMessage(Response& obj);

 private:
  Request m_request_data;
  Response m_response_data;
  Config m_config_data;
  // MethodHandler* m_method_handler; //이제 필요 없음?
  char* m_response_message;

 private:
};

#endif
