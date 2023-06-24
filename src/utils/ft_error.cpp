#include "Log.hpp"
#include "utils.hpp"

void print_error(const char *message)
{
  Log::print(ERROR, message);
}

void print_error(const char *message, int num)
{
  std::cout << message << " " << num << std::endl;
  return;
}

int ft_error(int status_code)
{
  print_error("Error!");
  return status_code;
}

int ft_error(int status_code, const char *error_message)
{
  print_error(error_message);
  return status_code;
}

void ft_error_exit(int exit_flag, const char *message)
{
  Log::print(ERROR, message);
  exit(exit_flag);
}
