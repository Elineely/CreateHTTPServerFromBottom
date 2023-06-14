#include "ResponseGenerator.hpp"
#include "Log.hpp"

#include <ctime>
#include <fstream>
#include <string>

// canonical
Response::Response()
{
  accepted_method = "";
  rediretion_location = "";
  file_path = "";
  file_name = "";
  root_path = "";
  cgi_bin_path = "";
  uploaded_path = "";
  index_name = "";
  index_exist = false;
  status_code = OK_200;
  redirection_exist = false;
  auto_index = false;
  file_exist = false;
  path_exist = false;
  cgi_flag = false;
  read_pipe_fd = -1;
  write_pipe_fd = -1;
  cgi_child_pid = -1;
  error_keyword = false;
  error_page_path = "";
}

Response::Response(const Response& obj)
{
  *this = obj;
}

Response::~Response() {}

Response& Response::operator=(const Response& obj)
{
  if (this != &obj)
  {
    accepted_method = obj.accepted_method;
    rediretion_location = obj.rediretion_location;
    file_path = obj.file_path;
    file_name = obj.file_name;
    root_path = obj.root_path;
    index_name = obj.index_name;
    index_exist = obj.index_exist;
    cgi_bin_path = obj.cgi_bin_path;
    uploaded_path = obj.uploaded_path;
    status_code = obj.status_code;
    redirection_exist = obj.redirection_exist;
    auto_index = obj.auto_index;
    file_exist = obj.file_exist;
    path_exist = obj.path_exist;
    body = obj.body;
    cgi_flag = obj.cgi_flag;
    read_pipe_fd = obj.read_pipe_fd;
    write_pipe_fd = obj.write_pipe_fd;
    cgi_child_pid = obj.cgi_child_pid;
    response_message = obj.response_message;
    error_keyword = obj.error_keyword;
    error_page_path = obj.error_page_path;
  }
  return (*this);
}

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
  std::string cgi_data(m_response.body.begin(), m_response.body.end());

  // get error code from cgi data
  std::string::size_type status_begin;
  std::string::size_type status_end;
  std::string cgi_status_code;
  if (cgi_data.size() == 0)
  {
    m_response.status_code = BAD_GATEWAY_502;
    throw m_response.status_code;
  }
  status_begin = cgi_data.find("Status: ");
  status_end = cgi_data.find("\r\n", status_begin);
  if (status_begin == std::string::npos && status_end == std::string::npos)
  {
    std::stringstream ss;
    int error_code;
    cgi_status_code = cgi_data.substr(status_begin + 8, 3);
    ss << cgi_status_code;
    ss >> error_code;
    throw(static_cast<StatusCode>(error_code));
  }

  // generate content-type header in case of cgi
  std::string::size_type content_type_begin;
  std::string::size_type content_type_end;
  content_type_begin = cgi_data.find("Content-Type: ");
  content_type_end = cgi_data.find("\r\n", content_type_begin);
  if (content_type_begin != std::string::npos &&
      content_type_end != std::string::npos)
    m_cgi_content_type = cgi_data.substr(content_type_begin,
                                         content_type_end - content_type_begin);

  // generate body in case of cgi
  std::string::size_type cgi_body_begin = cgi_data.find("\r\n\r\n");
  std::vector<char>::iterator iter =
      m_response.body.begin() + cgi_body_begin + 4;
  std::vector<char> cgi_body(iter, m_response.body.end());
  m_cgi_body = cgi_body;
}

ResponseGenerator::ResponseGenerator() {}
ResponseGenerator::ResponseGenerator(Request& request_data,
                                     Response& response_data)
// : m_request(request_data), m_response(response_data)
{
  m_request = request_data;
  m_response = response_data;
  if (response_data.auto_index == true && m_response.file_exist == false)
    m_target_file = "autoindex.html";
  else
    m_target_file = response_data.file_path + response_data.file_name;
  // Redirection 존재시 -->헤더??? GET으로 받았을 때만 redirection이 되나? 
  // method handler쪽에 redirection시 로직이 존재하는지?
  if (m_response.redirection_exist == true)
  {
    m_response.status_code = FOUND_302;
    m_response.body = std::vector<char>();
    m_target_file = "";
  }
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
  appendStrToResponse_message(" ");
  appendStrToResponse_message(status_str.getStatusStr(m_response.status_code));
}

void ResponseGenerator::generateContentType()
{
  if (m_response.cgi_flag == true)
  {
    appendStrToResponse_message(m_cgi_content_type);
    appendStrToResponse_message("\r\n");
  }
  else
  {
    appendStrToResponse_message("Content-Type:");
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
    ss << m_cgi_body.size();
  else
    ss << m_response.body.size();
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
  if (m_request.method == "HEAD") return;
  if (m_response.cgi_flag == true)
    m_response.response_message.insert(m_response.response_message.end(),
                                       m_cgi_body.begin(), m_cgi_body.end());
  else
    m_response.response_message.insert(m_response.response_message.end(),
                                       m_response.body.begin(),
                                       m_response.body.end());
  // m_response.response_message.push_back('\0');
}

std::vector<char> ResponseGenerator::generateErrorResponseMessage()
{
  setStartLine();
  generateErrorBody();
  setHeaders();
  setBody();

  return (m_response.response_message);
}

std::vector<char> ResponseGenerator::generateResponseMessage()
{
  try
  {
    LOG_DEBUG("m_response.status_code: %d", m_response.status_code);
    if (m_response.status_code != OK_200 && m_response.status_code != FOUND_302)
      throw(m_response.status_code);
    cgiDataProcess();
    setStartLine();
    setHeaders();
    setBody();
  }
  catch (StatusCode code)
  {
    LOG_ERROR("Error code: %d", code);
    m_response.status_code = code;
    generateErrorResponseMessage();
  }

  return (m_response.response_message);
}