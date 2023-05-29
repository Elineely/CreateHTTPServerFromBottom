#include "ResponseGenerator.hpp"

#include <fstream>
#include <string>

void ResponseGenerator::appendStrToResponse_message(std::string str)
{
  m_response.response_message.insert(m_response.response_message.end(),
                                     str.begin(), str.end());
}
void ResponseGenerator::appendStrToBody(std::string str)
{
  m_response.body.insert(m_response.body.end(), str.begin(), str.end());
}
std::string ResponseGenerator::statusCodeToString()
{
  std::stringstream ss;
  std::string status_code;

  ss << m_request.status;
  status_code = ss.str();
  return (status_code);
}

ResponseGenerator::ResponseGenerator() {}
ResponseGenerator::ResponseGenerator(Request& request_data,
                                     Response& response_data)
{
  m_request = request_data;
  m_response = response_data;
  m_target_file = response_data.file_path + response_data.file_name;
}
ResponseGenerator::ResponseGenerator(const ResponseGenerator& obj)
{
  m_request = obj.m_request;
  m_response = obj.m_response;
  m_target_file = obj.m_response.file_path + obj.m_response.file_name;
}
ResponseGenerator& ResponseGenerator::operator=(ResponseGenerator const& obj)
{
  if (this != &obj)
  {
    m_request = obj.m_request;
    m_response = obj.m_response;
    m_target_file = obj.m_response.file_path + obj.m_response.file_name;
  }
  return (*this);
}
ResponseGenerator::~ResponseGenerator(){};

void ResponseGenerator::generateVersion()
{
  appendStrToResponse_message("HTTP/1.1");
}
void ResponseGenerator::generateStatusCode()
{
  appendStrToResponse_message(" ");
  appendStrToResponse_message(statusCodeToString());
}
void ResponseGenerator::generateReasonPhrase()
{
  std::map<int, std::string> reason_map;
  std::string reason_phrase;

  // Inserting key-value pairs into the map
  reason_map.insert(std::make_pair(200, "OK"));
  reason_map.insert(std::make_pair(404, "NOT_FOUND"));

  appendStrToResponse_message(" ");
  appendStrToResponse_message(reason_map[m_request.status]);
  appendStrToResponse_message("\r\n");
}

void ResponseGenerator::generateContentType()
{
  appendStrToResponse_message("Content-Type: ");
  appendStrToResponse_message(Mime::getMime(m_target_file));
  appendStrToResponse_message("\r\n");
}
void ResponseGenerator::generateContentLength()
{
  if (m_response.body.size() <= 0) return;

  std::stringstream ss;
  std::string body_length;
  ss << m_response.body.size();
  body_length = ss.str();

  appendStrToResponse_message("Content-Length: ");
  appendStrToResponse_message(body_length);
  appendStrToResponse_message("\r\n");
}

void ResponseGenerator::generateErrorBody()
{
  appendStrToBody("<html>\r\n<head><title>");
  appendStrToBody(statusCodeToString());
  appendStrToBody(" ");
  appendStrToBody(StatusStr::getStatusStr(m_request.status));
  appendStrToBody(" ");
  appendStrToBody("</title></head>\r\n");
  appendStrToBody("<body>\r\n");
  appendStrToBody("<center><h1>");
  appendStrToBody(statusCodeToString());
  appendStrToBody(" ");
  appendStrToBody(StatusStr::getStatusStr(m_request.status));
  appendStrToBody("</h1></center>\r\n");
}

void ResponseGenerator::setStartLine()
{
  generateVersion();
  generateStatusCode();
  generateReasonPhrase();
  appendStrToResponse_message("\r\n");
}
void ResponseGenerator::setHeaders()
{
  generateContentType();
  generateContentLength();
  // headerConnection();
  // headerServer();
  // headerLocation();
  // headerDate();
  appendStrToResponse_message("\r\n");
}
void ResponseGenerator::setBody()
{
  if (m_request.method == "HEAD") return;
  m_response.response_message.insert(m_response.response_message.end(),
                                     m_response.body.begin(),
                                     m_response.body.end());
}

const char* ResponseGenerator::generateErrorResponseMessage()
{
  setStartLine();
  generateErrorBody();
  setHeaders();
  setBody();
}

const char* ResponseGenerator::generateResponseMessage()
{
  setStartLine();
  setHeaders();
  setBody();

  return (&m_response.response_message[0]);
}