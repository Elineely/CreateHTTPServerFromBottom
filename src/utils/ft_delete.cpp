#include "Request.hpp"
#include "ResponseGenerator.hpp"
#include "Server.hpp"
#include "utils.hpp"

void ft_delete_response(Response** response)
{
  if (*response == NULL)
  {
    return;
  }
  delete *response;
  *response = NULL;
}

void ft_delete_udata(t_event_udata** udata)
{
  if (*udata == NULL)
  {
    return;
  }
  delete *udata;
  *udata = NULL;
}

void ft_delete_request(Request** request)
{
  if (*request == NULL)
  {
    return;
  }
  delete *request;
  *request = NULL;
}