#ifndef HttpProcessor_HPP
#define HttpProcessor_HPP

#include "Config.hpp"
#include "PathFinder.hpp"
#include "Request.hpp"
#include "ResponseGenerator.hpp"
#include "MethodHandler.hpp"

class HttpProcessor
{
 public:
  HttpProcessor(Request& request_data, t_server& server_data);
  HttpProcessor(const HttpProcessor& obj);
  virtual ~HttpProcessor(void);
  HttpProcessor& operator=(HttpProcessor const& obj);
  struct Response& get_m_response(void);
  // Member Functions
//   char* generateMessage(Response& obj);

 private:

  void setErrorPage(t_server server_data, Response& response_data);
  HttpProcessor(void);
  struct Request m_request_data;
  struct Response m_response_data;
  t_server m_server_data;
  
  // MethodHandler* m_method_handler; //이제 필요 없음?
//   char* m_response_message;

};

#endif
