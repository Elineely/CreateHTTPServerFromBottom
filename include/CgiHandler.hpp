#ifndef CGIHANDLER_HPP
#define CGIHANDLER_HPP

#include <unistd.h>
#include <sys/wait.h>
#include <string>
#include <vector>

#include "Config.hpp"
#include "Request.hpp"
// #include "Response.hpp"
struct Response
{
  std::string accepted_method;
  bool redirection_exist;
  std::string redirection_location;
  bool auto_index;
  std::string file_name;
  bool file_exist;
  std::string file_path;
  bool path_exist;
  std::vector<char> body;
  StatusCode status_code;
  bool cgi_flag;
  std::string cgi_bin_path;
  std::string uploaded_path;
  std::vector<char> response_message;
  int pipe_fd;
};

// CgiHandler virtual class
class CgiHandler
{
 protected:
  Request m_request_data;
  struct t_server m_server_data;
  Response m_response_data;

  std::vector<std::string> m_env_list;
  std::vector<const char*> m_env_list_parameter;

  int m_to_child_fds[2];
  int m_to_parent_fds[2];
  pid_t m_pid;

  public : ///////////////////--------------------
  std::vector<char> m_content_vector;
// -------------------------------------

 public:
  CgiHandler(void);
  // // CgiHandler(/* ??? */);
  virtual ~CgiHandler(void);

  void setCgiEnv();

  virtual int pipeAndFork() = 0;
  virtual int executeCgi() = 0;
  virtual void getDataFromCgi() = 0;

  virtual void outsourceCgiRequest(void) = 0;

// 멀티CGI
  // 클라이언트로부터 들어오는 요청 -> 어떤 CGI 스크립트가 필요한지 결정하는 함수

// CGI 스크립트 실행 시
  // 서버가 인증을 요청하는 중이면, 스크립트 실행해서는 안 됨
  // 우리 인증함? 요청되었는지 확인?

// CGI의 출력값을 검증하는 함수 (CGI 입장에서 정상 출력 or 문제 발생)
    // 정상 출력 or 위치 설정 잘못된 경우 or 권한 or 헤더 이상의 경우 -> 4개 분기
    // CGI가 실패해도, 서버는 리스폰스의 책임이 있음

 private:
  CgiHandler(const CgiHandler& obj);
  CgiHandler& operator=(CgiHandler const& obj);
};


// /////////////////////////////////////////////////////////////
// class GetCgiHandler
class GetCgiHandler : public CgiHandler
{
 public:
  GetCgiHandler(void);
  // GetCgiHandler(/* ??? */);
  // GetCgiHandler(const GetCgiHandler& obj);
  // GetCgiHandler& operator=(GetCgiHandler const& obj);
  virtual ~GetCgiHandler(void);

  virtual void outsourceCgiRequest(void);

 private:

  virtual int pipeAndFork();
  virtual int executeCgi();
  virtual void getDataFromCgi();
};

// /////////////////////////////////////////////////////////////
// class PostCgiHandler
class PostCgiHandler : public CgiHandler
{
 public:
  PostCgiHandler(void);
  // // PostCgiHandler(/* ??? */);
  // PostCgiHandler(const PostCgiHandler& obj);
  // PostCgiHandler& operator=(PostCgiHandler const& obj);
  virtual ~PostCgiHandler(void);

  virtual void outsourceCgiRequest(void);

 private:
  virtual int pipeAndFork();
  virtual int executeCgi();
  virtual void getDataFromCgi();
};

#endif