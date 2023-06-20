#include "Mime.hpp"

Mime::Mime()
{
  m_mime_map.insert(std::make_pair(".html", "text/html"));
  m_mime_map.insert(std::make_pair(".htm", "text/html"));
  m_mime_map.insert(std::make_pair(".css", "text/css"));
  m_mime_map.insert(std::make_pair(".ico", "image/x-icon"));
  m_mime_map.insert(std::make_pair(".avi", "video/x-msvideo"));
  m_mime_map.insert(std::make_pair(".bmp", "image/bmp"));
  m_mime_map.insert(std::make_pair(".doc", "application/msword"));
  m_mime_map.insert(std::make_pair(".gif", "image/gif"));
  m_mime_map.insert(std::make_pair(".gz", "application/x-gzip"));
  m_mime_map.insert(std::make_pair(".ico", "image/x-icon"));
  m_mime_map.insert(std::make_pair(".jpg", "image/jpeg"));
  m_mime_map.insert(std::make_pair(".jpeg", "image/jpeg"));
  m_mime_map.insert(std::make_pair(".png", "image/png"));
  m_mime_map.insert(std::make_pair(".txt", "text/plain"));
  m_mime_map.insert(std::make_pair(".mp3", "audio/mp3"));
  m_mime_map.insert(std::make_pair(".pdf", "application/pdf"));
  m_mime_map.insert(std::make_pair("default", "text/html"));
}
Mime::~Mime() {}

std::string Mime::getMime(std::string target_file)
{
  std::string::size_type pos = target_file.rfind(".");
  std::string extension;
  if (!(pos == std::string::npos))
    extension = target_file.substr(pos);
  std::map<std::string, std::string>::iterator it = m_mime_map.find(extension);

  if (it != m_mime_map.end())
    return it->second;
  else
    return m_mime_map["default"];
}
