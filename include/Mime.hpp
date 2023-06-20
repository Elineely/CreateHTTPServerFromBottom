#ifndef MIME_HPP
#define MIME_HPP

#include <map>
#include <string>

class Mime
{
 private:
  std::map<std::string, std::string> m_mime_map;

 public:
  Mime();
  ~Mime();
  std::string getMime(std::string target_file);
};

#endif
