#include "Parser.hpp"
#include "utils.hpp"

#include <cstring>
#include <iostream>

// Default Constructor
Parser::Parser(void) {
  this->data_.first_line = NULL;
  this->pool_.total_line = NULL;
  this->pool_.line_len = 0;
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
char* Parser::ParseFirstLine(char* buf) {
  (void)buf;
  return (NULL);
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
    return ;
  }
  this->pool_.found_newline = true;
  this->pool_.offset = static_cast<size_t>((find + 2) - total_line);
}

// Public member functions
void Parser::ReadBuffer(char* buf) {
  try {
    this->SaveBufferInPool(buf);
    this->FindNewlineInPool();
    if (this->pool_.found_newline == false) {
      return ;
    }
    if (this->data_.first_line == NULL) {
      // this->data_.first_line = this->ParseFirstLine(buf);
      // std::cout << this->data_.first_line << std::endl;
    } else {
      // ParseHeader()
    }
  } catch(std::exception &e) {
    std::cout << e.what() << std::endl;
  }
}
