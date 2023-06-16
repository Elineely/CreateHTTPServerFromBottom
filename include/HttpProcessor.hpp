#ifndef HttpProcessor_HPP
#define HttpProcessor_HPP

#include "Config.hpp"
#include "MethodHandler.hpp"
#include "PathFinder.hpp"
#include "Request.hpp"
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
  t_server m_server_data;
};

#endif
