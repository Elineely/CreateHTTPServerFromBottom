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
Response::~Response()
{
  delete m_cgi_bin_path;
  // i를 어디까지 하고 삭제할지 생각해봐야할듯?
  for (int i = 0; i < 3; ++i) delete m_cgi_params[i];
  delete m_cgi_params;
};
std::string getMIME(std::string target_file)
{
  std::map<std::string, std::string> mime_map;

  mime_map.insert(std::make_pair(".html", "text/html"));
  mime_map.insert(std::make_pair(".htm", "text/html"));
  mime_map.insert(std::make_pair(".css", "text/css"));
  mime_map.insert(std::make_pair(".ico", "image/x-icon"));
  mime_map.insert(std::make_pair(".avi", "video/x-msvideo"));
  mime_map.insert(std::make_pair(".bmp", "image/bmp"));
  mime_map.insert(std::make_pair(".doc", "application/msword"));
  mime_map.insert(std::make_pair(".gif", "image/gif"));
  mime_map.insert(std::make_pair(".gz", "application/x-gzip"));
  mime_map.insert(std::make_pair(".ico", "image/x-icon"));
  mime_map.insert(std::make_pair(".jpg", "image/jpeg"));
  mime_map.insert(std::make_pair(".jpeg", "image/jpeg"));
  mime_map.insert(std::make_pair(".png", "image/png"));
  mime_map.insert(std::make_pair(".txt", "text/plain"));
  mime_map.insert(std::make_pair(".mp3", "audio/mp3"));
  mime_map.insert(std::make_pair(".pdf", "application/pdf"));
  mime_map.insert(std::make_pair("default", "text/html"));

  std::string extension = target_file.substr(target_file.rfind("."));
  std::map<std::string, std::string>::iterator it = mime_map.find(extension);

  if (it != mime_map.end())
    return it->second;  // Return the matched MIME type
  else
    return mime_map["default"];

  // std::string extension;
  // std::string mime;

  // extension = target_file.substr(target_file.rfind(".",
  // std::string::npos)); if (extension == ".html")
  //   mime = "text/html";
  // else if (extension == ".htm")
  //   mime = "text/html";
  // else if (extension == ".css")
  //   mime = "text/css";
  // else if (extension == ".ico")
  //   mime = "image/x-icon";
  // else if (extension == ".avi")
  //   mime = "video/x-msvideo";
  // else if (extension == ".bmp")
  //   mime = "image/bmp";
  // else if (extension == ".doc")
  //   mime = "application/msword";
  // else if (extension == ".gif")
  //   mime = "image/gif";
  // else if (extension == ".gz")
  //   mime = "application/x-gzip";
  // else if (extension == ".ico")
  //   mime = "image/x-icon";
  // else if (extension == ".jpg")
  //   mime = "image/jpeg";
  // else if (extension == ".jpeg")
  //   mime = "image/jpeg";
  // else if (extension == ".png")
  //   mime = "image/png";
  // else if (extension == ".txt")
  //   mime = "text/plain";
  // else if (extension == ".mp3")
  //   mime = "audio/mp3";
  // else if (extension == ".pdf")
  //   mime = "application/pdf";
  // else
  //   mime = "text/html";
}

void Response::generateVersion() { m_response_message.append("HTTP/1.1 "); }
void Response::generateStatusCode()
{
  std::stringstream ss;
  std::string status_code;

  ss << m_status_code;
  status_code = ss.str();
  m_response_message.append(status_code);
}
void Response::generateReasonPhrase()
{
  std::map<int, std::string> reason_map;
  std::string reason_phrase;

  // Inserting key-value pairs into the map
  reason_map.insert(std::make_pair(200, "OK"));
  reason_map.insert(std::make_pair(404, "NOT_FOUND"));

  m_response_message.append(" " + reason_map[m_status_code]);
}

void Response::generateContentType()
{
  m_response_message.append("Content-Type: ");
  m_response_message.append(getMIME(m_file_path));
  m_response_message.append("\r\n");
}
void Response::generateContentLength()
{
  std::stringstream ss;
  ss << m_body.length();
  m_response_message.append("Content-Length: ");
  m_response_message.append(ss.str());
  m_response_message.append("\r\n");
}

void Response::setStartLine()
{
  generateVersion();
  generateStatusCode();
  generateReasonPhrase();
  m_response_message.append("\r\n");
}
void Response::setHeaders()
{
  generateContentType();
  generateContentLength();
  // headerConnection();
  // headerServer();
  // headerLocation();
  // headerDate();
  m_response_message.append("\r\n");
}
void Response::setBody() { m_response_message.append(m_body); }
void Response::setErrorBody()
{
  // status코드에 따른 html문들을 파일 혹은 스트링으로 만들어서 집어넣어야할듯?
  // 거기에 따른 body를 붙이면 끝
}

const char* Response::generateErrorResponseMessage()
{
  setStartLine();
  setHeaders();
  setErrorBody();
}

const char* Response::generateResponseMessage()
{
  setStartLine();
  setHeaders();
  setBody();

  return (m_response_message.c_str());
}

std::string Response::get_m_file_path() { return (m_file_path); }
std::string Response::get_m_body() { return (m_body); }
StatusCode Response::get_m_status_code() { return (m_status_code); }
Request Response::get_m_request() { return (m_request); }
Config Response::get_m_config() { return (m_config); }
bool Response::get_m_cgi_flag() { return (m_cgi_flag); }
char* Response::get_m_cgi_bin_path() { return (m_cgi_bin_path); }
char** Response::get_m_cgi_params() { return (m_cgi_params); }
std::string Response::get_m_response_message() { return (m_response_message); }

void Response::set_m_file_path(const std::string obj) { m_file_path = obj; }
void Response::set_m_body(const std::string obj) { m_body = obj; }
void Response::set_m_status_code(const StatusCode obj) { m_status_code = obj; }
void Response::set_m_request(const Request obj) { m_request = obj; }
void Response::set_m_config(const Config obj) { m_config = obj; }
void Response::set_m_cgi_flag(const bool obj) { m_cgi_flag = obj; }
void Response::set_m_cgi_bin_path(const char* obj)
{
  int len;

  for (len = 0; len != NULL; ++len) obj[len];
  m_cgi_bin_path = new char[len + 1];
  for (int i = 0; i < len; ++i) m_cgi_bin_path[i] = obj[i];
  m_cgi_bin_path[len] = 0;
}
// 이중배열 편하게 복사하는방법은?
void Response::set_m_cgi_params(const char** obj) {}
void Response::set_m_response_message(const std::string obj)
{
  m_response_message = obj;
}