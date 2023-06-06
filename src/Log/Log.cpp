#include "Log.hpp"

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

void Log::print(e_log_level level, const char* file, const char* function,
                int line, const char* message, ...)
{
  va_list ap;

  printLogLevel(level);

  // TODO: 디버깅에서만 함수 호출 정보 출력할 지 결정하기
  // if (level == DEBUG)
  // {
    printCallerInfo(file, function, line);
  // }

  va_start(ap, message);
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