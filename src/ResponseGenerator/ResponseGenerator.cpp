#include "ResponseGenerator.hpp"

#include <algorithm>
#include <ctime>
#include <fstream>
#include <iostream>
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

  ss << m_response.status_code;
  status_code = ss.str();
  return (status_code);
}

void ResponseGenerator::cgiDataProcess()
{
  if (m_response.cgi_flag == false) return;
  // std::string cgi_data(m_response.body.begin(), m_response.body.end());

  if (m_response.body.size() == 0)
  {
    m_response.status_code = BAD_GATEWAY_502;
    throw m_response.status_code;
  }

  // get error code from cgi data
  std::string cgi_status_key = "Status:";
  std::vector<char>::iterator status_it;

  status_it = std::search(m_response.body.begin(), m_response.body.end(),
                          cgi_status_key.begin(), cgi_status_key.end());
  if (status_it == m_response.body.end())
  {
    m_response.status_code = OK_200;
  }
  else
  {
    std::stringstream ss;
    int status_code;

    while (status_it != m_response.body.end() && *status_it != '\r')
    {
      if (std::isdigit(*status_it))
      {
        ss << *status_it;
      }
      status_it++;
    }

    ss >> status_code;
    // cgi_status_code = status_it.substr(status_begin + 8, 3);
    // ss << status_it;
    // ss >> status_code;
    m_response.status_code = static_cast<StatusCode>(status_code);
  }

  // generate content-type header in case of cgi
  std::string content_type_key = "Content-Type:";
  std::vector<char>::iterator content_type_it;

  content_type_it =
      std::search(m_response.body.begin(), m_response.body.end(),
                  content_type_key.begin(), content_type_key.end());
  if (content_type_it == m_response.body.end())
  {
    m_response.status_code = INTERNAL_SERVER_ERROR_500;
    m_cgi_content_type = "text/html";
  }
  else
  {
    std::stringstream ss;
    std::vector<char>::iterator newline_it;

    std::advance(content_type_it, content_type_key.length());
    newline_it = std::find(content_type_it, m_response.body.end(), '\r');
    std::string content_type(content_type_it, newline_it);

    ss << ft_strtrim(content_type);
    ss >> m_cgi_content_type;
  }

  // generate body in case of cgi
  std::string body_start = "\r\n\r\n";
  std::vector<char>::iterator body_start_it;

  body_start_it = std::search(content_type_it, m_response.body.end(),
                          body_start.begin(), body_start.end());
  if (body_start_it == m_response.body.end())
  {
    m_cgi_body_it = m_response.body.begin();
  }
  else
  {
    std::advance(body_start_it, body_start.length());
    m_cgi_body_it = body_start_it;
  }
}

ResponseGenerator::ResponseGenerator(Request& request_data,
                                     Response& response_data)
    : m_request(request_data), m_response(response_data)
{
  if (response_data.auto_index == true && m_response.file_exist == false)
    m_target_file = "autoindex.html";
  else
    m_target_file = response_data.file_path + response_data.file_name;
  if (m_response.redirection_exist == true)
  {
    m_response.status_code = FOUND_302;
    m_response.body = std::vector<char>();
    m_target_file = "";
  }
}

ResponseGenerator::ResponseGenerator(const ResponseGenerator& obj)
    : m_request(obj.m_request), m_response(obj.m_response)
{
  m_target_file = obj.m_response.file_path + obj.m_response.file_name;
}

ResponseGenerator& ResponseGenerator::operator=(ResponseGenerator const& obj)
{
  if (this != &obj)
  {
    mime = obj.mime;
    status_str = obj.status_str;
    m_target_file = obj.m_response.file_path + obj.m_response.file_name;
    m_cgi_content_type = obj.m_cgi_content_type;
    m_cgi_body = obj.m_cgi_body;
  }
  return (*this);
}

ResponseGenerator::~ResponseGenerator() {}

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
  appendStrToResponse_message(" ");
  appendStrToResponse_message(status_str.getStatusStr(m_response.status_code));
}

void ResponseGenerator::generateContentType()
{
  appendStrToResponse_message("Content-Type:");
  if (m_response.cgi_flag == true)
  {
    if (m_cgi_content_type == "")
    {
      appendStrToResponse_message("text/html");
    }
    else
    {
      appendStrToResponse_message(m_cgi_content_type);
    }
    appendStrToResponse_message("\r\n");
  }
  else
  {
    appendStrToResponse_message(mime.getMime(m_target_file));
    appendStrToResponse_message("\r\n");
  }
}

void ResponseGenerator::generateContentLength()
{
  if (m_response.body.size() < 0) return;

  std::stringstream ss;
  std::string body_length;
  if (m_response.cgi_flag == true)
  {
    if (m_response.is_cgi_timeout == true)
    {
      ss << m_response.body.size();
    }
    else
    {
      size_t before_body_size;

      before_body_size = std::distance(m_response.body.begin(), m_cgi_body_it);
      ss << m_response.body.size() - before_body_size;
    }
  }
  else
  {
    ss << m_response.body.size();
  }
  body_length = ss.str();

  appendStrToResponse_message("Content-length:");
  appendStrToResponse_message(body_length);
  appendStrToResponse_message("\r\n");
}

void ResponseGenerator::generateServer()
{
  appendStrToResponse_message("Server:");
  appendStrToResponse_message("Cute_webserv/1.0 (");
  std::string my_os;
#if defined(_WIN32)
  my_os = "Windows";
#elif defined(__linux__)
  my_os = "Linux";
#elif defined(__APPLE__) && defined(__MACH__)
  my_os = "Mac OS";
#elif defined(__FreeBSD__)
  my_os = "FreeBSD";
#else
  my_os = "Unknown OS";
#endif
  appendStrToResponse_message(my_os);
  appendStrToResponse_message(")");
  appendStrToResponse_message("\r\n");
}

void ResponseGenerator::generateDate()
{
  std::time_t currentTime = std::time(nullptr);
  std::tm* timeInfo = std::gmtime(&currentTime);

  char buffer[30];
  std::strftime(buffer, sizeof(buffer), "%a, %d %b %Y %H:%M:%S GMT", timeInfo);

  appendStrToResponse_message("Date: ");
  appendStrToResponse_message(buffer);
  appendStrToResponse_message("\r\n");
}

void ResponseGenerator::generateLocation()
{
  if (m_response.redirection_exist == false) return;
  appendStrToResponse_message("Location: ");
  appendStrToResponse_message(m_response.rediretion_location);
  appendStrToResponse_message("\r\n");
}

void ResponseGenerator::generateErrorBody()
{
  if (m_response.error_keyword == true &&
      m_response.error_page_vector.size() > 0)
  {
    m_response.body = m_response.error_page_vector;
  }
  else
  {
    appendStrToBody("<html>\r\n<head><title>");
    appendStrToBody(statusCodeToString());
    appendStrToBody(" ");
    appendStrToBody(status_str.getStatusStr(m_response.status_code));
    appendStrToBody(" ");
    appendStrToBody("</title></head>\r\n");
    appendStrToBody("<body>\r\n");
    appendStrToBody("<center><h1>");
    appendStrToBody(statusCodeToString());
    appendStrToBody(" ");
    appendStrToBody(status_str.getStatusStr(m_response.status_code));
    appendStrToBody("</h1></center></body>\r\n</html>");
  }
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
  generateServer();
  generateDate();
  generateContentLength();
  generateContentType();
  // headerConnection();
  generateLocation();
  appendStrToResponse_message("\r\n");
}

void ResponseGenerator::setBody()
{
  if (m_request.method == "HEAD")
  {
    return;
  }
  if (m_response.cgi_flag == true)
  {
    if (m_response.is_cgi_timeout == true)
    {
      m_response.response_message.insert(m_response.response_message.end(),
                                         m_response.body.begin(),
                                         m_response.body.end());
    }
    else
    {
      m_response.response_message.insert(m_response.response_message.end(),
                                         m_cgi_body_it, m_response.body.end());
    }
  }
  else
  {
    m_response.response_message.insert(m_response.response_message.end(),
                                       m_response.body.begin(),
                                       m_response.body.end());
  }
}

std::vector<char> ResponseGenerator::generateErrorResponseMessage()
{
  setStartLine();
  generateErrorBody();
  setHeaders();
  setBody();

  return (m_response.response_message);
}

// statuc code가 비정상이라면 내부적으로 generateErrorResponseMessage함수가
// 호출되고 m_response.response_message를 생성 해줌
// 1. cgi관련 request라면 1차 가공을 한 후
// 2. start line생성
// 3. header들 생성
// 4. body 생성
std::vector<char>& ResponseGenerator::generateResponseMessage()
{
  try
  {
    if (m_response.status_code != OK_200 && m_response.status_code != FOUND_302)
      throw(m_response.status_code);
    cgiDataProcess();
    setStartLine();
    setHeaders();
    setBody();
  }
  catch (StatusCode code)
  {
    m_response.status_code = code;
    generateErrorResponseMessage();
  }

  return (m_response.response_message);
}
