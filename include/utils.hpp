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

struct Request;
struct Response;
struct t_event_udata;

std::vector<std::string> ft_config_split(std::string readLine);

// 소문자 변환 펑터 객체
struct tolower_functor
{
  char operator()(char c) const { return std::tolower(c); }
};

std::string ft_strtrim(const std::string &str);
std::vector<std::string> ft_split(const std::string &str, char delimiter,
                                  int count);
std::string ft_toLower(const std::string &str);

int ft_error(int status_code);
int ft_error(int status_code, const char *error_message);
void ft_error_exit(int exit_flag, const char *error_message);
void ft_process_print(const char *str);

template <typename T>
void ft_delete(T** ptr)
{
  if (*ptr == NULL)
  {
    return;
  }
  delete *ptr;
  *ptr = NULL;
}

#endif
