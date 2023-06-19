#ifndef HttpProcessor_HPP
#define HttpProcessor_HPP

#include "CgiHandler.hpp"
#include "Config.hpp"
#include "Log.hpp"
#include "MethodHandler.hpp"
#include "PathFinder.hpp"
#include "Request.hpp"
#include "Response.hpp"
#include "ResponseGenerator.hpp"

class HttpProcessor
{
 public:
  Request& m_request_data;
  Response& m_response_data;

  HttpProcessor(Request& request_data, Response& response_data,
                t_server& server_data);
  HttpProcessor(const HttpProcessor& obj);
  virtual ~HttpProcessor(void);
  HttpProcessor& operator=(HttpProcessor const& obj);
  struct Response& get_m_response(void);

 private:
  void setErrorPage(t_server server_data, Response& response_data);
  HttpProcessor(void);

  // MethodHandler* m_method_handler; //이제 필요 없음?
  //   char* m_response_message;
};

#endif
