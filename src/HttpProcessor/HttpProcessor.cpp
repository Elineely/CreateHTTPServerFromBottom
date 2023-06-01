#include "HttpProcessor.hpp"

HttpProcessor::HttpProcessor(void) {}

HttpProcessor::HttpProcessor(Request& request_data, t_server& server_data)
{
  // parsing의 유효성 검사(일반적) -> 상태코드 업데이트
  // 굳이 responsegernerator가 갖고있을 필요가 있을까 싶음.
  m_request_data = request_data;
  m_server_data = server_data;
  MethodHandler* method_handler;

  method_handler = &GetMethodHandler(m_request_data, m_config_data);
  method_handler->generateResponse();
  // if (request_data.method == "GET")
  //   method_handler = &GetMethodHandler(request_data, config_data);
  // else if (request_data.method == "POST")
  //   method_handler = &PostMethodHandler(request_data, config_data);
  // else if (request_data.method == "DELETE")
  //   method_handler = &DeleteMethodHandler(request_data, config_data);
  // else
  // {
  //   // 상태코드 업데이트
  // }
  // try
  // {
  //   if (m_reuest_data.status_c)
  //     throw stsu
  //   m_method_handler.doSomething(); -> response 구조체 채워짐
  //   GeneratoresponseMessage();
  // }
  // catch(StatusCode code_Num)
  // {
  //   errorPageGenerator(Code_NUm);
  //    m_response_message = errorPageGenerator.getMesaage();
  // }
}
// Copy Constructor
HttpProcessor::HttpProcessor(const HttpProcessor& obj) {}

// Destructor
HttpProcessor::~HttpProcessor(void) {}

Copy Assignment Operator HttpProcessor& HttpProcessor::operator=(
    HttpProcessor const& obj)
{
}