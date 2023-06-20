#include "StatusCode.hpp"

StatusStr::StatusStr()
{
  m_str_map.insert(std::make_pair(200, "OK"));
  m_str_map.insert(std::make_pair(201, "Created"));
  m_str_map.insert(std::make_pair(400, "Bad Request"));
  m_str_map.insert(std::make_pair(404, "Not Found"));
  m_str_map.insert(std::make_pair(405, "Method Not Allowed"));
  m_str_map.insert(std::make_pair(408, "Request Timeout"));
  m_str_map.insert(std::make_pair(413, "Payload Too Large"));
  m_str_map.insert(std::make_pair(414, "URI Too Long"));
  m_str_map.insert(std::make_pair(429, "Too Many Requests"));
  m_str_map.insert(std::make_pair(500, "Internal Server Error"));
  m_str_map.insert(std::make_pair(501, "Not Implemented"));
  m_str_map.insert(std::make_pair(502, "Bad Gateway"));
  m_str_map.insert(std::make_pair(504, "Gateway Timeout"));
  m_str_map.insert(std::make_pair(505, "HTTP Version Not Supported"));
}
StatusStr::~StatusStr() {}

std::string StatusStr::getStatusStr(StatusCode code)
{
  std::map<int, std::string>::iterator it = m_str_map.find(code);

  if (it != m_str_map.end())
    return it->second;
  else
    return m_str_map[200];
}
