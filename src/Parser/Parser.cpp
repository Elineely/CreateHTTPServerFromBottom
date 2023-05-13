#include "Parser.hpp"

#include <cstring>
#include <iostream>
#include <sstream>
#include <string>

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
  std::map<std::string, std::string> headers = this->data_.headers;
  size_t offset;

  offset = this->pool_.offset;
  total_line = this->pool_.total_line;
  find = std::strstr(total_line + offset, "\r\n");
  if (find == NULL) {
    return (false);
  }
  this->pool_.prev_offset = offset;
  if (std::strncmp(total_line + offset - 2, "\r\n\r\n", 4) == 0) {
    if (this->data_.method == "POST" &&
        (headers.find("content-length") != headers.end() ||
         headers["transfer-encoding"] == "chunked")) {
      this->data_.validation_phase = ON_BODY;
    } else {
      this->data_.validation_phase = COMPLETE;
    }
    this->pool_.offset = offset + 2;
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

void Parser::ParseBody(std::vector<char>& body) {
  char* total_line = this->pool_.total_line;
  size_t offset = this->pool_.offset;
  size_t line_len = this->pool_.line_len;
  long long content_length =
      std::strtoll(this->data_.headers["content-length"].c_str(), NULL, 10);

  if (content_length < 0) {
    this->data_.status = BAD_REQUEST_400;
    throw std::invalid_argument("Content-length should be positive value");
  }

  body.reserve(line_len - offset);

  for (size_t idx = offset; idx < line_len; idx += 1) {
    body.push_back(*(total_line + idx));
  }

  if (static_cast<size_t>(content_length) != body.size()) {
    this->data_.status = BAD_REQUEST_400;
    throw std::invalid_argument(
        "Content-length and body length should be equal");
  }
  this->data_.validation_phase = COMPLETE;
  std::cout << "Body:";
  for (std::vector<char>::iterator it = body.begin(); it != body.end(); it++) {
    std::cout << *it;
  }
  std::cout << std::endl;
}

void Parser::ParseChunkedBody(std::vector<char>& body) {
  while (true) {
    char* body_curr = this->pool_.total_line + this->pool_.prev_offset;
    char* find = std::strstr(body_curr, "\r\n");
    if (find == NULL) {
      break;
    }
    std::string str_chunk_size(body_curr,
                               static_cast<size_t>(find - body_curr));
    long long chunk_size = std::stoll(str_chunk_size);
    std::cout << "chunk_size:" << chunk_size << std::endl;
    if (str_chunk_size != "0" && chunk_size <= 0) {
      this->data_.status = BAD_REQUEST_400;
      throw std::invalid_argument("Chunk size should be positive value");
    }
    char* next_newline = std::strstr(find + 2, "\r\n");
    if (next_newline == NULL) {
      break;
    }
    if (static_cast<long long>(next_newline - (find + 2)) != chunk_size) {
      this->data_.status = BAD_REQUEST_400;
      throw std::invalid_argument("Chunk body should be equal with chunk size");
    }
    for (char* ptr = find + 2; ptr < next_newline; ptr += 1) {
      body.push_back(*ptr);
    }

    for (std::vector<char>::iterator it = body.begin(); it != body.end();
         it++) {
      std::cout << *it;
    }

    std::cout << std::endl;

    this->pool_.prev_offset = next_newline - this->pool_.total_line + 2;
    this->pool_.offset = this->pool_.prev_offset;
  }
}

// Public member functions
void Parser::ReadBuffer(char* buf) {
  try {
    std::map<std::string, std::string>& headers = this->data_.headers;
    if (this->data_.validation_phase == COMPLETE) {
      return;
    }

    // 클라이언트로 받은 데이터를 Pool 에 저장
    this->SaveBufferInPool(buf);
    // 마지막으로 CRLF 를 찾은 지점 이후에 CRLF 가 들어왔는지 확인
    if (this->FindNewlineInPool() == false &&
        this->data_.validation_phase != ON_BODY) {
      return;
    }

    // Validation 단계에 따라 first-line, header, [body] 를 파싱
    switch (this->data_.validation_phase) {
      case READY:
        this->ParseFirstLine();
        std::cout << this->data_.method << std::endl;
        std::cout << this->data_.uri << std::endl;
        std::cout << this->data_.http_version << std::endl;
        break;
      case ON_HEADER:
        this->ParseHeaders(headers);
        break;
      case ON_BODY:
        // TODO: Body 에서 "HELLO\0WORLD" 와 같은 문자열이 들어왔을 때 어떻게
        // 처리할 지? (char* vs std::string)
        if (headers.find("content-length") != headers.end()) {
          this->ParseBody(this->data_.body);
        } else if (headers["transfer-encoding"] == "chunked") {
          this->ParseChunkedBody(this->data_.body);
        }

      default:
        break;
    }
  } catch (std::exception& e) {
    std::cout << e.what() << std::endl;
  }
}
