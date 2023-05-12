#include "Parser.hpp"

#include <cstring>
#include <iostream>
#include <sstream>

#include "utils.hpp"

// Default Constructor
Parser::Parser(void) {
  this->data_.validation_phase = READY;
  this->pool_.total_line = NULL;
  this->pool_.line_len = 0;
  this->pool_.prev_offset = 0;
  this->pool_.offset = 0;
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
  if (uri.at(0) != '/') {
    this->data_.status = BAD_REQUEST_400;
    throw std::invalid_argument("URI should start with slash(/)");
  }

  http_version =
      input.substr(idx2 + 1, input.find_first_of('\r', idx2 + 1) - idx2 - 1);
  if (http_version != "HTTP/1.0" && http_version != "HTTP/1.1") {
    this->data_.status = BAD_REQUEST_400;
    throw std::invalid_argument("HTTP version should be either 1.0 or 1.1");
  }
  this->data_.method = method;
  this->data_.uri = uri;
  this->data_.http_version = http_version;
  this->data_.validation_phase = ON_HEADER;
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
    // TODO: strjoin 보다 효율적인 문자열 병합 방법이 있는지 찾기
    this->pool_.total_line = ft_strjoin(temp_buf, buf);
    delete temp_buf;
  }
}

bool Parser::FindNewlineInPool(void) {
  const char* find;
  const char* total_line;
  size_t offset;

  offset = this->pool_.offset;
  total_line = this->pool_.total_line;
  find = std::strstr(total_line + offset, "\r\n");
  if (find == NULL) {
    return (false);
  }
  this->pool_.prev_offset = offset;
  if (std::strncmp(total_line + offset - 2, "\r\n\r\n", 4) == 0) {
    // TODO: chunked body 인 경우도 확인하기
    if (this->data_.method == "POST" &&
        this->data_.headers.find("content-length") !=
            this->data_.headers.end()) {
      this->data_.validation_phase = ON_BODY;
    } else {
      this->data_.validation_phase = COMPLETE;
    }
    this->pool_.offset = static_cast<size_t>((find + 4) - total_line);
  } else {
    this->pool_.offset = static_cast<size_t>((find + 2) - total_line);
  }
  return (true);
}

void Parser::ParseHeaders(std::map<std::string, std::string>& headers) {
  do {
    size_t offset = this->pool_.offset;
    size_t prev_offset = this->pool_.prev_offset;
    // '\r\n' 을 포함해서 저장
    std::string input(this->pool_.total_line, prev_offset,
                      offset - prev_offset);
    std::vector<std::string> vec;
    std::string key;
    std::string value;

    vec = ft_split(input, ':', 1);
    if (vec.size() != 2) {
      this->data_.status = BAD_REQUEST_400;
      throw std::invalid_argument("HTTP header value should be one");
    }

    key = ft_toLower(vec[KEY]);
    if (headers.find(key) != headers.end()) {
      this->data_.status = BAD_REQUEST_400;
      throw std::invalid_argument("HTTP header field should not be duplicated");
    }

    value = ft_strtrim(vec[VALUE]);
    headers[key] = value;
    std::cout << "key:" << key << "/ value:" << value << std::endl;
  } while (this->FindNewlineInPool() == true &&
           this->data_.validation_phase == ON_HEADER);
}

// Public member functions
void Parser::ReadBuffer(char* buf) {
  try {
    // CRLF 가 2번으로 마무리되면 더 이상 받지 않도록 처리
    this->SaveBufferInPool(buf);
    if (this->FindNewlineInPool() == false) {
      return;
    }
    switch (this->data_.validation_phase) {
      case READY:
        this->ParseFirstLine();
        std::cout << this->data_.method << std::endl;
        std::cout << this->data_.uri << std::endl;
        std::cout << this->data_.http_version << std::endl;
        break;
      case ON_HEADER:
        this->ParseHeaders(this->data_.headers);
        break;
        // TODO: Body 에서 "HELLO\0WORLD" 와 같은 문자열이 들어왔을 때 어떻게
        // 처리할 지? (char* vs std::string)

      default:
        break;
    }
  } catch (std::exception& e) {
    std::cout << e.what() << std::endl;
  }
}
