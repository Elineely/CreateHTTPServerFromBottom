#include "../../include/utils.hpp"

std::string ft_strtrim(const std::string &str) {
  std::string trimmed_str(str);

  // trim whitespace at the beginning of the string
  std::string::iterator it = trimmed_str.begin();
  while (it != trimmed_str.end() && std::isspace(*it)) {
    ++it;
  }
  trimmed_str.erase(trimmed_str.begin(), it);

  // trim whitespace at the end of the string
  it = trimmed_str.end();
  while (it != trimmed_str.begin() && std::isspace(*(it - 1))) {
    --it;
  }
  trimmed_str.erase(it, trimmed_str.end());

  return trimmed_str;
}
