#include "../../include/utils.hpp"

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

