#include "Server.hpp"

int Server::getKqueue()
{
  int kq = kqueue();
  if (kq == -1)
  {
    ft_error_exit(EXIT_FAILURE, strerror(errno));
  }
  return kq;
}
