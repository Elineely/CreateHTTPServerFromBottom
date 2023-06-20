#ifndef STATUS_CODE_HPP
#define STATUS_CODE_HPP

#include <map>
#include <string>

enum StatusCode
{
  NO_PROBLEM = 0,
  OK_200 = 200,
  CREATED_201 = 201,
  FOUND_302 = 302,
  BAD_REQUEST_400 = 400,
  NOT_FOUND_404 = 404,
  METHOD_NOT_ALLOWED_405 = 405,
  REQUEST_TIMEOUT_408 = 408,
  PAYLOAD_TOO_LARGE_413 = 413,
  URI_TOO_LONG_414 = 414,
  TOO_MANY_REQUESTS_429 = 429,
  INTERNAL_SERVER_ERROR_500 = 500,
  NOT_IMPLEMENTED_501 = 501,
  BAD_GATEWAY_502 = 502,
  GATEWAY_TIMEOUT_504 = 504,
  HTTP_VERSION_NOT_SUPPORTED_505 = 505
};

class StatusStr
{
 private:
  std::map<int, std::string> m_str_map;

 public:
  StatusStr();
  ~StatusStr();
  std::string getStatusStr(StatusCode code);
};

#endif
