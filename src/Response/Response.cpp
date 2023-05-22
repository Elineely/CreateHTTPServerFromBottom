#include "Response.hpp"

#include <fstream>
#include <string>

Response::Response()
{
  m_file_path = "";
  m_body = "";
  m_status_code = NO_PROBLEM;
  m_request = Request();
  m_config = Config();
  m_cgi_flag = false;
  m_cgi_bin_path = NULL;
  m_cgi_params = NULL;
}
Response::Response(Request& request_data, Config& config_data)
{
  m_file_path = "";
  m_body = "";
  m_status_code = NO_PROBLEM;
  m_request = request_data;
  m_config = config_data;
  m_cgi_flag = false;
  m_cgi_bin_path = NULL;
  m_cgi_params = NULL;
}
Response::Response(const Response& obj)
{
  m_file_path = obj.m_file_path;
  m_body = obj.m_body;
  m_status_code = obj.m_status_code;
  m_request = obj.m_request;
  m_config = obj.m_config;
  m_cgi_flag = obj.m_cgi_flag;
  m_cgi_bin_path = obj.m_cgi_bin_path;
  m_cgi_params = obj.m_cgi_params;
}
Response& Response::operator=(Response const& obj)
{
  if (this != &obj)
  {
    m_file_path = obj.m_file_path;
    m_body = obj.m_body;
    m_status_code = obj.m_status_code;
    m_request = obj.m_request;
    m_config = obj.m_config;
    m_cgi_flag = obj.m_cgi_flag;
    m_cgi_bin_path = obj.m_cgi_bin_path;
    m_cgi_params = obj.m_cgi_params;
  }
  return (*this);
}

std::string statusCodeToString(StatusCode obj)
{
  std::stringstream ss;
  std::string code;
  std::string reason;

  // 하드코딩으로 되어있는데 나중에 map으로 고쳐야함.
  switch (obj)
  {
    case OK_200:
      reason = "OK";
      break;

    default:
      break;
  }
  ss << obj;
  code = ss.str();
  return (code + " " + reason);
}
std::string getMIME(std::string target_file)
{
  std::string extension;
  std::string mime;

  extension = target_file.substr(target_file.rfind(".", std::string::npos));
  if (extension == ".html")
    mime = "text/html";
  else if (extension == ".htm")
    mime = "text/html";
  else if (extension == ".css")
    mime = "text/css";
  else if (extension == ".ico")
    mime = "image/x-icon";
  else if (extension == ".avi")
    mime = "video/x-msvideo";
  else if (extension == ".bmp")
    mime = "image/bmp";
  else if (extension == ".doc")
    mime = "application/msword";
  else if (extension == ".gif")
    mime = "image/gif";
  else if (extension == ".gz")
    mime = "application/x-gzip";
  else if (extension == ".ico")
    mime = "image/x-icon";
  else if (extension == ".jpg")
    mime = "image/jpeg";
  else if (extension == ".jpeg")
    mime = "image/jpeg";
  else if (extension == ".png")
    mime = "image/png";
  else if (extension == ".txt")
    mime = "text/plain";
  else if (extension == ".mp3")
    mime = "audio/mp3";
  else if (extension == ".pdf")
    mime = "application/pdf";
  else
    mime = "text/html";
  return (mime);
}

void Response::generateHeaderContentType()
{
  m_response_message.append("Content-Type: ");
  m_response_message.append(getMIME(m_file_path));
  m_response_message.append("\r\n");
}
void Response::generateHeaderContentLength()
{
  std::stringstream ss;
  ss << m_body.length();
  m_response_message.append("Content-Length: ");
  m_response_message.append(ss.str());
  m_response_message.append("\r\n");
}

void Response::makeBody()
{
  std::ifstream fileStream(m_file_path);

  if (fileStream)
  {
    std::stringstream buffer;
    buffer << fileStream.rdbuf();
    m_body = buffer.str();
    fileStream.close();
  }
  else
    throw NOT_FOUND_404;
}

void Response::setStartLine()
{
  m_response_message.append("HTTP/1.1 ");
  m_response_message.append(statusCodeToString(this->m_request.status));
  m_response_message.append("\r\n");
}
void Response::setHeaders()
{
  generateHeaderContentType();
  generateHeaderContentLength();
  // headerConnection();
  // headerServer();
  // headerLocation();
  // headerDate();
  m_response_message.append("\r\n");
}
void Response::setBody() { m_response_message.append(m_body); }
