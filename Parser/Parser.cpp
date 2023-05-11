#include "Parser.hpp"

#include <cstring>
#include <iostream>
#include <sstream>

#include "utils.hpp"

// Default Constructor
Parser::Parser(void) {
  this->data_.first_line_validation = NOT_INSPECTED;
  this->pool_.total_line = NULL;
  this->pool_.line_len = 0;
  this->pool_.prev_offset = 0;
  this->pool_.offset = 0;
  this->pool_.found_newline = false;
}

// Copy Constructor
Parser::Parser(const Parser& src) { (void)src; }

// Destructor
Parser::~Parser(void) {
  if (this->pool_.total_line != NULL) {
    delete this->pool_.total_line;
  }
}

// Copy Assignment Operator
Parser& Parser::operator=(Parser const& rhs) {
  (void)rhs;
  if (this != &rhs) {
  }
  return *this;
}

// Private member functions
void Parser::ParseFirstLine(void) {
  size_t prev_offset = this->pool_.prev_offset;
  size_t offset = this->pool_.offset;
  // \r\n 은 제외하고 input 에 저장
  std::string input(this->pool_.total_line, prev_offset,
                    offset - prev_offset - 2);
  std::string method;
  std::string uri;
  std::string http_version;
  size_t idx1;
  size_t idx2;

  idx1 = input.find_first_of(' ', 0);
  method = input.substr(0, idx1);
  std::cout << "method:" << method << std::endl;
  if (method != "GET" && method != "POST" && method != "DELETE") {
    this->data_.status = BAD_REQUEST_400;
    throw std::invalid_argument("Method is not acceptable");
  }
  idx2 = input.find_first_of(' ', idx1 + 1);
  if (idx2 == std::string::npos) {
    this->data_.status = BAD_REQUEST_400;
    throw std::invalid_argument("There should be a whitespace after method");
  }
  uri = input.substr(idx1 + 1, idx2 - idx1 - 1);
  std::cout << "uri:" << uri << std::endl;
  if (uri.at(0) != '/') {
    this->data_.status = BAD_REQUEST_400;
    throw std::invalid_argument("URI should start with slash(/)");
  }
  http_version =
      input.substr(idx2 + 1, input.find_first_of('\r', idx2 + 1) - idx2 - 1);
  std::cout << "http_version:" << http_version << std::endl;
  if (http_version != "HTTP/1.0" && http_version != "HTTP/1.1") {
    this->data_.status = BAD_REQUEST_400;
    throw std::invalid_argument("HTTP version should be either 1.0 or 1.1");
  }
  this->data_.method = method;
  this->data_.uri = uri;
  this->data_.http_version = http_version;
  this->data_.first_line_validation = VALID;
}

// QUESTION: vector 의 push_back 처럼 size 를 2배씩 늘려야 효율이 좋을까?
void Parser::SaveBufferInPool(char* buf) {
  size_t buf_len;
  char* temp_buf;

  if (buf == NULL) {
    throw std::invalid_argument("NULL string is not allowed");
  }
  buf_len = std::strlen(buf);
  this->pool_.line_len += buf_len;
  if (this->pool_.total_line == NULL) {
    this->pool_.total_line = new char[buf_len + 1]();
    std::memmove(this->pool_.total_line, buf, buf_len + 1);
  } else {
    temp_buf = ft_strdup(this->pool_.total_line);
    delete this->pool_.total_line;
    this->pool_.total_line = ft_strjoin(temp_buf, buf);
    delete temp_buf;
  }
}

void Parser::FindNewlineInPool(void) {
  const char* find;
  const char* total_line;
  size_t offset;

  offset = this->pool_.offset;
  total_line = this->pool_.total_line;
  find = std::strstr(total_line + offset, "\r\n");
  if (find == NULL) {
    this->pool_.found_newline = false;
    return;
  }
  this->pool_.found_newline = true;
  this->pool_.prev_offset = offset;
  this->pool_.offset = static_cast<size_t>((find + 2) - total_line);
}

// Public member functions
void Parser::ReadBuffer(char* buf) {
  try {
    this->SaveBufferInPool(buf);
    this->FindNewlineInPool();
    if (this->pool_.found_newline == false) {
      return;
    }
    if (this->data_.first_line_validation == NOT_INSPECTED) {
      this->ParseFirstLine();
      std::cout << this->data_.method << std::endl;
      std::cout << this->data_.uri << std::endl;
      std::cout << this->data_.http_version << std::endl;
    } else {
      // ParseHeader()
    }
  } catch (std::exception& e) {
    std::cout << e.what() << std::endl;
  }
}
