#ifndef CGIHANDLER_HPP
#define CGIHANDLER_HPP

#include <unistd.h>
#include <sys/wait.h>
#include <string>
#include <vector>

#include "Request.hpp"
#include "Response.hpp"

// CgiHandler virtual class
class CgiHandler
{
 protected:
  Request m_request_data;
  // struct t_server m_server_data;
  Response m_response_data;

  // 시스템 환경변수(필요한가?) + 서버 환경변수 붙여주는 함수
  // 사용자 설정 환경변수 추가하는 함수
  std::vector<std::string> m_env_list;

  int m_to_child_fds[2];
  int m_to_parent_fds[2];
  pid_t m_pid;


  std::vector<char> m_content_vector;

 public:
  CgiHandler(void);
  CgiHandler(/* ??? */);
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

// 환경변수 이름은 운영체제에 정의되어 있지만, 그 값은 서버가 MUST 채워줘야 한다
  // 환경변수,, X_ 로 대체 뭐가 들어가는 건데

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
  GetCgiHandler(/* ??? */);
  GetCgiHandler(const GetCgiHandler& obj);
  GetCgiHandler& operator=(GetCgiHandler const& obj);
  virtual ~GetCgiHandler(void);

  virtual int pipeAndFork();
  virtual int executeCgi();
  virtual void getDataFromCgi();

  virtual void outsourceCgiRequest(void);

 private:
  GetCgiHandler(void);
};

// /////////////////////////////////////////////////////////////
// class PostCgiHandler
class PostCgiHandler : public CgiHandler
{
 public:
  PostCgiHandler(/* ??? */);
  PostCgiHandler(const PostCgiHandler& obj);
  PostCgiHandler& operator=(PostCgiHandler const& obj);
  virtual ~PostCgiHandler(void);

  virtual int pipeAndFork();
  virtual int executeCgi();
  virtual void getDataFromCgi();

  virtual void outsourceCgiRequest(void);

  // 바디 읽기 전에 content_length를 확인하고, 파일 만들기 전에 content_type 확인해야 함

 private:
  PostCgiHandler(void);
};


// /////////////////////////////////////////////////////////////
// class DeleteCgiHandler
class DeleteCgiHandler : public CgiHandler
{
 public:
  DeleteCgiHandler(/* ??? */);
  DeleteCgiHandler(const DeleteCgiHandler& obj);
  DeleteCgiHandler& operator=(DeleteCgiHandler const& obj);
  virtual ~DeleteCgiHandler(void);

  virtual int pipeAndFork();
  virtual int executeCgi();
  virtual void getDataFromCgi();

  virtual void outsourceCgiRequest(void);

// 실행 전에 server_protocol이 HTTP/1.1 이상인지 확인해야 함

// 바디 내용이 삭제되면, content_length도 반영되어야 함

 private:
  DeleteCgiHandler(void);
};

#endif