#include "Log.hpp"
#include "Server.hpp"

#include <iomanip>

void Log::start(void)
{
    std::cout << "=========================================================================" << std::endl;
    std::cout << GREEN_TEXT << "WELCOME TO" << CUT_TEXT << std::endl;
    std::cout << " _____         _          _    _        _                             \n";
    std::cout << "/  __ \\       | |        | |  | |      | |                            \n";
    std::cout << "| /  \\/ _   _ | |_   ___ | |  | |  ___ | |__   ___   ___  _ __ __   __\n";
    std::cout << "| |    | | | || __| / _ \\| |\\/| | / _ \\| '_ \\ / __| / _ \\| '__|\\ \\ / /\n";
    std::cout << "| \\__/\\| |_| || |_ |  __/\\  /\\  /|  __/| |_) |\\__ \\|  __/| |    \\ V / \n";
    std::cout << " \\____/ \\__,_| \\__| \\___| \\/  \\/  \\___||_.__/ |___/ \\___||_|     \\_/  \n";
    std::cout << "                                                                     \n";
    std::cout << "=========================================================================" << std::endl;
}

void Log::printRequestResult(t_event_udata* current_udata)
{
  std::cout << BLUE_TEXT << std::setw(6) << std::left << current_udata->m_request->method << CUT_TEXT;
  std::cout << std::setw(current_udata->m_request->uri.size() + 2) << std::left << current_udata->m_request->uri;
  if (current_udata->m_response->status_code >= 400)
  {
    std::cout << RED_TEXT;
  }
  else
  {
    std::cout << GREEN_TEXT;
  }
  std::cout << std::setw(3) << std::left << current_udata->m_response->status_code << CUT_TEXT << std::endl;
}

void Log::printCallerInfo(const char* file, const char* function, int line)
{
  std::ostringstream log_info_stream;
  std::string log_info_str;

  log_info_stream << "[" << file << ":" << line << "][" << function << "]";
  log_info_str = log_info_stream.str() + " ";

  std::cout << log_info_str;
}

void Log::printLogLevel(e_log_level level)
{
  switch (level)
  {
    case INFO:
      std::cout << BLUE_TEXT << "[INFO] " << CUT_TEXT;
      break;
    case DEBUG:
      std::cout << GREEN_TEXT << "[DEBUG] " << CUT_TEXT;
      break;
    case ERROR:
      std::cout << RED_TEXT << "[ERROR] " << CUT_TEXT;
      break;
    default:
      break;
  }
}

void Log::print(e_log_level level, const char* message, ...)
{
  va_list ap;

  va_start(ap, message);

  printLogLevel(level);

  for (const char* p = message; *p != '\0'; ++p)
  {
    if (*p == '%')
    {
      ++p;

      switch (*p)
      {
        case 'd':
          std::cout << va_arg(ap, int);
          break;
        case 'f':
          std::cout << va_arg(ap, double);
          break;
        case 's':
          std::cout << va_arg(ap, const char*);
          break;
        default:
          std::cout << "[Invalid format specifier]";
          break;
      }
    }
    else
    {
      std::cout << *p;
    }
  }

  std::cout << std::endl;

  va_end(ap);
}

void Log::print_line(e_log_level level, const char* file, const char* func, int line, const char* message, ...)
{
  va_list ap;

  va_start(ap, message);

  printLogLevel(level);
  printCallerInfo(file, func, line);

  for (const char* p = message; *p != '\0'; ++p)
  {
    if (*p == '%')
    {
      ++p;

      switch (*p)
      {
        case 'd':
          std::cout << va_arg(ap, int);
          break;
        case 'f':
          std::cout << va_arg(ap, double);
          break;
        case 's':
          std::cout << va_arg(ap, const char*);
          break;
        default:
          std::cout << "[Invalid format specifier]";
          break;
      }
    }
    else
    {
      std::cout << *p;
    }
  }

  std::cout << std::endl;

  va_end(ap);
}
