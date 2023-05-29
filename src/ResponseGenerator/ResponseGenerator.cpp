#include "ResponseGenerator.hpp"

#include <fstream>
#include <string>

ResponseGenerator::ResponseGenerator() {}
ResponseGenerator::ResponseGenerator(Request& request_data,
                                     Response& response_data)
{
  m_request = request_data;
  m_response = response_data;
}
ResponseGenerator::ResponseGenerator(const ResponseGenerator& obj)
{
  m_request = obj.m_request;
  m_response = obj.m_response;
}
ResponseGenerator& ResponseGenerator::operator=(ResponseGenerator const& obj)
{
  if (this != &obj)
  {
    m_request = obj.m_request;
    m_response = obj.m_response;
  }
  return (*this);
}
ResponseGenerator::~ResponseGenerator(){};

void ResponseGenerator::generateVersion()
{
  m_response_message.append("HTTP/1.1 ");
}
void ResponseGenerator::generateStatusCode()
{
  std::stringstream ss;
  std::string status_code;

  ss << m_status_code;
  status_code = ss.str();
  m_response_message.append(status_code);
}
void ResponseGenerator::generateReasonPhrase()
{
  std::map<int, std::string> reason_map;
  std::string reason_phrase;

  // Inserting key-value pairs into the map
  reason_map.insert(std::make_pair(200, "OK"));
  reason_map.insert(std::make_pair(404, "NOT_FOUND"));

  m_response_message.append(" " + reason_map[m_status_code]);
}

void ResponseGenerator::generateContentType()
{
  m_response_message.append("Content-Type: ");
  m_response_message.append(getMIME(m_file_path));
  m_response_message.append("\r\n");
}
void ResponseGenerator::generateContentLength()
{
  std::stringstream ss;
  ss << m_body.length();
  m_response_message.append("Content-Length: ");
  m_response_message.append(ss.str());
  m_response_message.append("\r\n");
}

void ResponseGenerator::setStartLine()
{
  generateVersion();
  generateStatusCode();
  generateReasonPhrase();
  m_response_message.append("\r\n");
}
void ResponseGenerator::setHeaders()
{
  generateContentType();
  generateContentLength();
  // headerConnection();
  // headerServer();
  // headerLocation();
  // headerDate();
  m_response_message.append("\r\n");
}
void ResponseGenerator::setBody() { m_response_message.append(m_body); }
void ResponseGenerator::setErrorBody()
{
  // status코드에 따른 html문들을 파일 혹은 스트링으로 만들어서 집어넣어야할듯?
  // 거기에 따른 body를 붙이면 끝
}

const char* ResponseGenerator::generateErrorResponseMessage()
{
  setStartLine();
  setHeaders();
  setErrorBody();
}

const char* ResponseGenerator::generateResponseMessage()
{
  setStartLine();
  setHeaders();
  setBody();

  return (&m_response.response_message[0]);
}