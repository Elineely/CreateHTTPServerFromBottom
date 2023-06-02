#ifndef HttpProcessor_HPP
#define HttpProcessor_HPP

#include "Config.hpp"
#include "PathFinder.hpp"
#include "Request.hpp"
#include "Response.hpp"
#include "MethodHandler.hpp"

class HttpProcessor
{
 public:
  HttpProcessor(Request& request_data, t_server& server_data);
  HttpProcessor(const HttpProcessor& obj);
  virtual ~HttpProcessor(void);
  HttpProcessor& operator=(HttpProcessor const& obj);

  // Member Functions
//   char* generateMessage(Response& obj);

 private:
  HttpProcessor(void);
  Request m_request_data;
  Response m_response_data;
  t_server m_server_data;
  
  // MethodHandler* m_method_handler; //이제 필요 없음?
//   char* m_response_message;

};

#endif
