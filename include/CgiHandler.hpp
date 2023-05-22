

// CgiHandler class
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

/* 공통 함수 = 0 */

 private:
  CgiHandler(const CgiHandler& obj);
  CgiHandler& operator=(CgiHandler const& obj);
}


// class GetCgiHandler
class GetCgiHandler : public CgiHandler
{
 public:
  GetCgiHandler(const GetCgiHandler& obj);
  GetCgiHandler(/* ??? */);
  virtual ~GetCgiHandler(void);
  GetCgiHandler& operator=(GetCgiHandler const& obj);

 private:
  GetCgiHandler(void);
};


// class PostCgiHandler
class PostCgiHandler : public CgiHandler
{
 public:
  PostCgiHandler(const PostCgiHandler& obj);
  PostCgiHandler(/* ??? */);
  virtual ~PostCgiHandler(void);
  PostCgiHandler& operator=(PostCgiHandler const& obj);

 private:
  PostCgiHandler(void);
};


// class DeleteCgiHandler
class DeleteCgiHandler : public CgiHandler
{
 public:
  DeleteCgiHandler(const DeleteCgiHandler& obj);
  DeleteCgiHandler(/* ??? */);
  virtual ~DeleteCgiHandler(void);
  DeleteCgiHandler& operator=(DeleteCgiHandler const& obj);

 private:
  DeleteCgiHandler(void);
};
