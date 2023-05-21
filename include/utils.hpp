#ifndef UTILS_HPP
#define UTILS_HPP

#include <algorithm>
#include <cctype>
#include <cstring>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

std::vector<std::string> ft_config_split(std::string readLine);

// 소문자 변환 펑터 객체
struct tolower_functor
{
  char operator()(char c) const { return std::tolower(c); }
};

char *ft_strjoin(char const *s1, char const *s2);
size_t ft_strlen(const char *str);
char *ft_strdup(const char *src);
std::string ft_strtrim(const std::string &str);
std::vector<std::string> ft_split(const std::string &str, char delimiter,
                                  int count);
std::string ft_toLower(const std::string &str);

int ft_error(int status_code);
int ft_error(int status_code, const char *error_message);
void ft_error_exit(int exit_flag, const char *error_message);
void ft_process_print(const char *str);

#endif
