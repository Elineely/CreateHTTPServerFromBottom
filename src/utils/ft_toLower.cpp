#include "../../include/utils.hpp"

std::string ft_toLower(const std::string &str) {
  std::string result(str);

  std::transform(result.begin(), result.end(), result.begin(),
                 tolower_functor());

  return (result);
}
