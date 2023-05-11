#ifndef utils_H
# define utils_H

#include <cstring>
#include <string>
#include <vector>
#include <sstream>

char* ft_strjoin(char const* s1, char const* s2);
size_t ft_strlen(const char* str);
char	*ft_strdup(const char* src);
std::string ft_strtrim(const std::string& str);
std::vector<std::string> ft_split(const std::string& str, char delimiter);

#endif
