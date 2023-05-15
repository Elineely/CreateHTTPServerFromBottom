// copyright 2023 joonhan

#define BUF_SIZE 512
#include "Parser.hpp"

int main(void) {

  int case_num = 2;
  char start_line[BUF_SIZE];
  char headers[BUF_SIZE];
  char body[BUF_SIZE];

  switch (case_num) {
    case 1:
      // CASE1. content-length 로 들어오는 경우
      std::strcpy(start_line, "POST /index.html HTTP/1.1\r\n");
      std::strcpy(headers,
                  "Server: Cute Web Server \r\nDate: Wed, 11 May 2023 09:10:00 "
                  "KST\r\nContent-length:10\r\n\r\n");
      std::strcpy(body, "helloworld");
      break;
    case 2:
      // CASE2. chunked body 로 들어오는 경우
      std::strcpy(start_line, "POST /index.html HTTP/1.1\r\n");
      std::strcpy(headers,
                  "Server: Cute Web Server \r\nDate: Wed, 11 May 2023 09:10:00 "
                  "KST\r\nTransfer-Encoding: chunked\r\n\r\n");
      std::strcpy(body, "5\r\nhello\r\n5\r\nworld\r\n0\r\n");
      // std::strcpy(body, "helloworld");
      break;
    case 3:
      // CASE3. CRLF 로 끝나는 경우
      std::strcpy(start_line, "GET /index.html HTTP/1.1\r\n");
      std::strcpy(headers,
                  "Server: Cute Web Server \r\nDate: Wed, 11 May 2023 09:10:00 "
                  "KST\r\n\r\n");
      break;
    default:
      break;
  }

  // ===============================
  Parser parser;
  parser.ReadBuffer(start_line);
  parser.ReadBuffer(headers);
  parser.ReadBuffer(body);
}
