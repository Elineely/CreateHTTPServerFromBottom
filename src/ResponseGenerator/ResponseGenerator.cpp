#include <fstream>
#include <string>

#include "ResponseGenerator.hpp"

ResponseGenerator::ResponseGenerator()
{
  _target_file = "";
  _body = "";
  response_message = "";
}
ResponseGenerator::ResponseGenerator(Request& request_data, Config& config_data)
{
  _target_file = "";
  _body = "";
  response_message = "";

  // request구조체의 복사대입연산자 필요
  request = request_data;
  config = config_data;
}
ResponseGenerator::ResponseGenerator(const ResponseGenerator& obj)
{
  _target_file = obj._target_file;
  _body = obj._body;
  response_message = obj.response_message;
  request = obj.request;
  config = obj.config;
}
ResponseGenerator& ResponseGenerator::operator=(ResponseGenerator const& obj)
{
  if (this != &obj)
  {
    _target_file = obj._target_file;
    _body = obj._body;
    response_message = obj.response_message;
    request = obj.request;
    config = obj.config;
  }
  return (*this);
}

std::string statusCodeToString(StatusCode obj)
{
  std::stringstream ss;
  std::string code;
  std::string reason;

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

void ResponseGenerator::headerContentType()
{
  response_message.append("Content-Type: ");
  response_message.append(getMIME(_target_file));
  response_message.append("\r\n");
}
void ResponseGenerator::headerContentLength()
{
  std::stringstream ss;
  ss << _body.length();
  response_message.append("Content-Length: ");
  response_message.append(ss.str());
  response_message.append("\r\n");
}

void ResponseGenerator::makeBody()
{
  std::ifstream fileStream(_target_file);

  if (fileStream)
  {
    std::stringstream buffer;
    buffer << fileStream.rdbuf();
    _body = buffer.str();
    fileStream.close();
  }
  else
    throw NOT_FOUND;
}

void ResponseGenerator::setStartLine()
{
  response_message.append("HTTP/1.1 ");
  response_message.append(statusCodeToString(this->request.status));
  response_message.append("\r\n");
}
void ResponseGenerator::setHeaders()
{
  headerContentType();
  headerContentLength();
  // headerConnection();
  // headerServer();
  // headerLocation();
  // headerDate();
  response_message.append("\r\n");
}
void ResponseGenerator::setBody() { response_message.append(_body); }
