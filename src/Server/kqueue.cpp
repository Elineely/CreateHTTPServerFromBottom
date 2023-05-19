#include "Server.hpp"

int Server::getKqueue()
{
  int kq = kqueue();
  if (kq == -1)
  {
    ft_error_exit(1, strerror(errno));
  }
  return kq;
}