
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
  virtual void giveDataToResponse(void) = 0;

 private:
  CgiHandler(const CgiHandler& obj);
  CgiHandler& operator=(CgiHandler const& obj);
}


// class GetCgiHandler
class GetCgiHandler : public CgiHandler
{
 public:
  GetCgiHandler(/* ??? */);
  GetCgiHandler(const GetCgiHandler& obj);
  GetCgiHandler& operator=(GetCgiHandler const& obj);
  virtual ~GetCgiHandler(void);

  virtual void outsourceCgiRequest(void);
  virtual void giveDataToResponse(void);

  // cgi_bin_path(cgi binary 파일 위치), cgi_envp(cgi 실행 프로그램에 넘겨줄 환경 변수) 필요

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

 private:
  DeleteCgiHandler(void);
};
