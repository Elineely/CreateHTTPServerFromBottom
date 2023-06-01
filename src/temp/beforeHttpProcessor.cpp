#include "MethodHandler.hpp"
#include "Request.hpp"
#include "ResponseGenerator.hpp"

void printVector(std::vector<char> vec)
{
  for (int i = 0; i < vec.size(); ++i) std::cout << vec[i];
  std::cout << std::endl;
}
void fillRequest(Request &request_data)
{
  request_data.status = OK_200;
  request_data.uri = "/index.html";
  request_data.http_version = "HTTP/1.1";
}
void fillResponse(Response &response_data)
{
  response_data.auto_index = true;
  response_data.file_name = "a.txt";
  response_data.file_path = "./test/";
  response_data.file_exist = true;
  response_data.path_exist = true;
  response_data.cgi_flag = false;
  response_data.redirection_exist = false;
  // response_data.rediretion_location = "https:// www.naver.com/";
}

std::vector<char> getHttpMessage()
{
  Request request_data;
  Response response_data;
  fillRequest(request_data);
  fillResponse(response_data);

  MethodHandler *handler;
  handler = new GetMethodHandler(request_data, response_data);
  handler->methodRun();

  request_data = handler->get_m_request_data();
  response_data = handler->get_m_response_data();
  ResponseGenerator response_generator(request_data, response_data);

  return response_generator.generateResponseMessage();
}