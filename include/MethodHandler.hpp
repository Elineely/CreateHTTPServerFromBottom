#ifndef METHODHANDLER_HPP
#define METHODHANDLER_HPP

#include "Config.hpp"
#include "Request.hpp"
#include "Response.hpp"

class MethodHandler
{
 protected:
  Request m_request_data;
  Config m_config_data;
  Response m_response_data;
  std::string m_body;

  MethodHandler(const MethodHandler& obj);
  MethodHandler& operator=(MethodHandler const& obj);
  virtual void makeBody() = 0;

 public:
  MethodHandler(void);
  MethodHandler(const Request& request_data, const Config& config_data);
  virtual ~MethodHandler(void);
};

// PostMethodHandler, DeleteMethodHandler도 만들기
class GetMethodHandler : public MethodHandler
{
 public:
  GetMethodHandler();
  GetMethodHandler(const GetMethodHandler& obj);
  GetMethodHandler(const Request& request_data, const Config& config_data);
  virtual ~GetMethodHandler(void);
  GetMethodHandler& operator=(GetMethodHandler const& obj);

  void makeBody();

 private:
  // GetMethodHandler(void);
};

#endif

// class PostMethodHandler : public MethodHandler
// {
//  public:
//   PostMethodHandler(void);
//   PostMethodHandler(const PostMethodHandler& obj);
//   PostMethodHandler(const Request& request_data, const Config& config_data);
//   virtual ~PostMethodHandler(void);
//   PostMethodHandler& operator=(PostMethodHandler const& obj);
//   void generateResponse(void);

//  private:
//   PostMethodHandler(void);
// };

// class DeleteMethodHandler : public MethodHandler
// {
//  public:
//   DeleteMethodHandler(void);
//   DeleteMethodHandler(const DeleteMethodHandler& obj);
//   DeleteMethodHandler(const Request& request_data, const Config&
//   config_data); virtual ~DeleteMethodHandler(void); DeleteMethodHandler&
//   operator=(DeleteMethodHandler const& obj);

//   void generateResponse(void);

//  private:
//   DeleteMethodHandler(void);
// };
