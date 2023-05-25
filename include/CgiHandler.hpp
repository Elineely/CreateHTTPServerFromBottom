#ifndef CGIHANDLER_HPP
#define CGIHANDLER_HPP

#include <unistd.h>
#include <sys/wait.h>
#include <string>
#include <vector>

// CgiHandler virtual class
class CgiHandler
{
 protected:
//   Request m_request_data;
//   Config m_config_data;
//   Response m_response_data;

 public:
  CgiHandler(void);
  CgiHandler(/* ??? */);
  virtual ~CgiHandler(void);

  virtual void outsourceCgiRequest(void) = 0;

// CGI의 출력값을 검증하는 함수
    // 정상 출력 or 위치 설정 잘못된 경우 or 권한 or 헤더 이상의 경우 -> 4개 분기
//CGI로부터 정상 출력 받았을 때 : 헤더와 body 분리(\n\n)? or 통째로?

 private:
  CgiHandler(const CgiHandler& obj);
  CgiHandler& operator=(CgiHandler const& obj);
};


// class GetCgiHandler
class GetCgiHandler : public CgiHandler
{
 public:
  GetCgiHandler(/* ??? */);
  GetCgiHandler(const GetCgiHandler& obj);
  GetCgiHandler& operator=(GetCgiHandler const& obj);
  virtual ~GetCgiHandler(void);

  virtual void outsourceCgiRequest(void);

 private:
  GetCgiHandler(void);
};


// class PostCgiHandler
class PostCgiHandler : public CgiHandler
{
 public:
  PostCgiHandler(/* ??? */);
  PostCgiHandler(const PostCgiHandler& obj);
  PostCgiHandler& operator=(PostCgiHandler const& obj);
  virtual ~PostCgiHandler(void);

  virtual void outsourceCgiRequest(void);

 private:
  PostCgiHandler(void);
};


// class DeleteCgiHandler
class DeleteCgiHandler : public CgiHandler
{
 public:
  DeleteCgiHandler(/* ??? */);
  DeleteCgiHandler(const DeleteCgiHandler& obj);
  DeleteCgiHandler& operator=(DeleteCgiHandler const& obj);
  virtual ~DeleteCgiHandler(void);

  virtual void outsourceCgiRequest(void);

 private:
  DeleteCgiHandler(void);
};

#endif