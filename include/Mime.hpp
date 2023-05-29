#ifndef MIME_HPP
#define MIME_HPP

#include <map>
#include <string>

class Mime
{
 private:
  static std::map<std::string, std::string> m_mime_map;

  Mime(const Mime& obj);
  Mime& operator=(Mime const& obj);
  ~Mime();

 public:
  Mime();
  static std::string getMime(std::string target_file);
};

#endif