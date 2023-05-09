#include "Parser.hpp"
#include <iostream>

Parser::Parser(void) { this->data_.first_line = NULL; }

Parser::Parser(const Parser& src) { (void)src; }

Parser::~Parser(void) {}

Parser& Parser::operator=(Parser const& rhs) {
  (void)rhs;
  if (this != &rhs) {
  }
  return *this;
}

char* Parser::ParseFirstLine(char* buf) {

}

void Parser::ReadBuffer(char* buf) {
  if (this->data_.first_line == NULL) {

    // this->data_.first_line = this->ParseFirstLine(buf);
    std::cout << this->data_.first_line << std::endl;
  } else {
    // ParseHeader()
  }
}
