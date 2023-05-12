#include "utils.hpp"

size_t ft_strlen(const char *str) {
  size_t len;

  len = 0;
  if (str == NULL) {
    return (0);
  }
  while (str[len] != '\0') {
    len += 1;
  }
  return (len);
}

char *ft_strjoin(char const *s1, char const *s2) {
  size_t s1_len;
  size_t s2_len;
  char *result;

  if (s1 == NULL && s2 == NULL) {
    return (NULL);
  }
  s1_len = ft_strlen(s1);
  s2_len = ft_strlen(s2);
  result = new char[s1_len + s2_len + 1]();
  if (result == NULL) {
    return (NULL);
  }
  memmove(result, s1, s1_len);
  memmove(result + s1_len, s2, s2_len + 1);
  return (result);
}

char *ft_strdup(const char *src) {
  size_t idx;
  size_t src_len;
  char *result;

  idx = 0;
  src_len = ft_strlen(src);
  result = new char[src_len + 1]();
  if (result == NULL) {
    return (NULL);
  }
  while (*(src + idx) != '\0') {
    *(result + idx) = *(src + idx);
    idx += 1;
  }
  *(result + idx) = '\0';
  return (result);
}

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

std::vector<std::string> ft_split(const std::string &str, char delimiter,
                                  int count) {
  std::string buf;
  std::stringstream ss(str);
  std::vector<std::string> vec;

  for (int curr = 0; curr < count; curr += 1) {
    if (std::getline(ss, buf, delimiter)) {
      vec.push_back(buf);
    }
  }

  if (std::getline(ss, buf, '\n')) {
    vec.push_back(buf);
  }

  return (vec);
}

std::string ft_toLower(const std::string &str) {
  std::string result(str);

  std::transform(result.begin(), result.end(), result.begin(),
                 tolower_functor());

  return (result);
}
