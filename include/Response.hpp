#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include <iostream>
#include <string>
#include <vector>

#include "StatusCode.hpp"
struct Response
{
  std::string accepted_method;
  bool redirection_exist;
  std::string rediretion_location;
  bool auto_index;
  std::string file_name;
  bool file_exist;
  std::string index_name;
  bool index_exist;
  std::string file_path;
  bool path_exist;
  std::string root_path;
  std::vector<char> body;
  StatusCode status_code;
  bool cgi_flag;
  bool is_cgi_timeout;
  std::string cgi_bin_path;
  std::string uploaded_path;
  std::vector<char> response_message;
  std::vector<char> error_page_vector;
  int static_read_file_fd;
  off_t static_read_file_size;
  int static_write_file_fd;
  int read_pipe_fd;
  int write_pipe_fd;
  int cgi_child_pid;
  bool error_keyword;

  Response()
  {
    accepted_method = "";
    rediretion_location = "";
    file_path = "";
    file_name = "";
    root_path = "";
    cgi_bin_path = "";
    uploaded_path = "";
    index_name = "";
    index_exist = false;
    status_code = OK_200;
    redirection_exist = false;
    auto_index = false;
    file_exist = false;
    path_exist = false;
    cgi_flag = false;
    is_cgi_timeout = false;
    read_pipe_fd = -1;
    write_pipe_fd = -1;
    cgi_child_pid = -1;
    error_keyword = false;
    static_read_file_fd = -1;
    static_write_file_fd = -1;
    static_read_file_size = 0;
  }
  Response(const Response& obj) { *this = obj; }
  ~Response(){};
  Response& operator=(const Response& obj)
  {
    if (this != &obj)
    {
      accepted_method = obj.accepted_method;
      rediretion_location = obj.rediretion_location;
      file_path = obj.file_path;
      file_name = obj.file_name;
      root_path = obj.root_path;
      index_name = obj.index_name;
      index_exist = obj.index_exist;
      cgi_bin_path = obj.cgi_bin_path;
      uploaded_path = obj.uploaded_path;
      status_code = obj.status_code;
      redirection_exist = obj.redirection_exist;
      auto_index = obj.auto_index;
      file_exist = obj.file_exist;
      path_exist = obj.path_exist;
      body = obj.body;
      cgi_flag = obj.cgi_flag;
      is_cgi_timeout = obj.is_cgi_timeout;
      read_pipe_fd = obj.read_pipe_fd;
      write_pipe_fd = obj.write_pipe_fd;
      cgi_child_pid = obj.cgi_child_pid;
      response_message = obj.response_message;
      error_keyword = obj.error_keyword;
      error_page_vector = obj.error_page_vector;
      static_read_file_fd = obj.static_read_file_fd;
      static_write_file_fd = obj.static_write_file_fd;
      static_read_file_size = obj.static_read_file_size;
    }
    return (*this);
  }
};

#endif
