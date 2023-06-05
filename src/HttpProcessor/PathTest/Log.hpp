#ifndef LOG_HPP
#define LOG_HPP

#include <cstdarg>
#include <iostream>
#include <string>

#define RED_TEXT 	"\033[31m"
#define GREEN_TEXT 	"\033[32m"
#define YELLOW_TEXT	"\033[33m"
#define BLUE_TEXT	"\033[34m"
#define CUT_TEXT	"\033[0m"

class Log
{
 public:
  static void error(const char* message, ...);
  static void info(const char* message, ...);
  static void debug(const char* message, ...);
  static void start(void);
  static void print(const char* message, va_list ap);
};

#endif