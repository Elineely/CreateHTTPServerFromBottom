#include "Log.hpp"

void Log::error(const char* message, ...)
{
  va_list ap;
  va_start(ap, message);

  std::cout << RED_TEXT << "[ERROR] " << CUT_TEXT;

  print(message, ap);

  va_end(ap);
}

void Log::info(const char* message, ...)
{
  va_list ap;
  va_start(ap, message);

  std::cout << BLUE_TEXT << "[INFO] " << CUT_TEXT;

  print(message, ap);

  va_end(ap);
}

void Log::debug(const char* message, ...)
{
  va_list ap;
  va_start(ap, message);

  std::cout << GREEN_TEXT << "[DEBUG] " << CUT_TEXT;

  print(message, ap);

  va_end(ap);
}

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

void Log::print(const char* message, va_list ap)
{
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
}