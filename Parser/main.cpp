// copyright 2023 joonhan

// #define BUF_SIZE 512
#include "Parser.hpp"

int main(void) {
  // char buf[BUF_SIZE];

  Parser parser;
  // struct Request client_request;
  char start_line[] = "GET /index.html HTTP/1.1\r\n";

  parser.ReadBuffer(start_line);
}
