#include "Log.hpp"
#include "Server.hpp"

#include <iomanip>

void Log::start(void)
{
    std::cerr << "=========================================================================" << std::endl;
    std::cerr << GREEN_TEXT << "WELCOME TO" << CUT_TEXT << std::endl;
    std::cerr << " _____         _          _    _        _                             \n";
    std::cerr << "/  __ \\       | |        | |  | |      | |                            \n";
    std::cerr << "| /  \\/ _   _ | |_   ___ | |  | |  ___ | |__   ___   ___  _ __ __   __\n";
    std::cerr << "| |    | | | || __| / _ \\| |\\/| | / _ \\| '_ \\ / __| / _ \\| '__|\\ \\ / /\n";
    std::cerr << "| \\__/\\| |_| || |_ |  __/\\  /\\  /|  __/| |_) |\\__ \\|  __/| |    \\ V / \n";
    std::cerr << " \\____/ \\__,_| \\__| \\___| \\/  \\/  \\___||_.__/ |___/ \\___||_|     \\_/  \n";
    std::cerr << "                                                                     \n";
    std::cerr << "=========================================================================" << std::endl;
}

void Log::printRequestResult(t_event_udata* current_udata)
{
  std::cerr << BLUE_TEXT << std::setw(6) << std::left << current_udata->m_request->method << CUT_TEXT;
  std::cerr << std::setw(current_udata->m_request->uri.size() + 2) << std::left << current_udata->m_request->uri;
  if (current_udata->m_response->status_code >= 400)
  {
    std::cerr << RED_TEXT;
  }
  else
  {
    std::cerr << GREEN_TEXT;
  }
  std::cerr << std::setw(3) << std::left << current_udata->m_response->status_code << CUT_TEXT << std::endl;
}

void Log::printCallerInfo(const char* file, const char* function, int line)
{
  std::ostringstream log_info_stream;
  std::string log_info_str;

  log_info_stream << "[" << file << ":" << line << "][" << function << "]";
  log_info_str = log_info_stream.str() + " ";

  std::cerr << log_info_str;
}

void Log::printLogLevel(e_log_level level)
{
  switch (level)
  {
    case INFO:
      std::cerr << BLUE_TEXT << "[INFO] " << CUT_TEXT;
      break;
    case DEBUG:
      std::cerr << GREEN_TEXT << "[DEBUG] " << CUT_TEXT;
      break;
    case ERROR:
      std::cerr << RED_TEXT << "[ERROR] " << CUT_TEXT;
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
          std::cerr << va_arg(ap, int);
          break;
        case 'f':
          std::cerr << va_arg(ap, double);
          break;
        case 's':
          std::cerr << va_arg(ap, const char*);
          break;
        default:
          std::cerr << "[Invalid format specifier]";
          break;
      }
    }
    else
    {
      std::cerr << *p;
    }
  }

  std::cerr << std::endl;

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
          std::cerr << va_arg(ap, int);
          break;
        case 'f':
          std::cerr << va_arg(ap, double);
          break;
        case 's':
          std::cerr << va_arg(ap, const char*);
          break;
        default:
          std::cerr << "[Invalid format specifier]";
          break;
      }
    }
    else
    {
      std::cerr << *p;
    }
  }

  std::cerr << std::endl;

  va_end(ap);
}
