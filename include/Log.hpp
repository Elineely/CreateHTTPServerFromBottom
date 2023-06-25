#ifndef LOG_HPP
#define LOG_HPP

#include <cstdarg>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>

#define RED_TEXT 	"\033[31m"
#define GREEN_TEXT 	"\033[32m"
#define YELLOW_TEXT	"\033[33m"
#define BLUE_TEXT	"\033[34m"
#define CUT_TEXT	"\033[0m"

// #define LOG_ERROR(format, args...) Log::print_line(ERROR, __FILE__, __func__, __LINE__, format, ## args);
// #define LOG_DEBUG(format, args...) Log::print_line(DEBUG, __FILE__, __func__, __LINE__, format, ## args);
// #define LOG_INFO(format, args...) Log::print_line(INFO, __FILE__, __func__, __LINE__, format, ## args);

struct t_event_udata;

enum e_log_level
{
  ERROR,
  DEBUG,
  INFO
};

class Log
{
  private:
    Log(void);
    ~Log();
    Log(const Log& src);
    Log& operator=(const Log& rhs);

    static void printLogLevel(e_log_level level);
    static void printCallerInfo(const char* file, const char* function, int line);

  public:
    static void start(void);
    static void print(e_log_level level, const char* message, ...);
    static void printRequestResult(t_event_udata* current_udata);
    static void print_line(e_log_level level, const char* file, const char* func, int line, const char* message, ...);
};


#endif
