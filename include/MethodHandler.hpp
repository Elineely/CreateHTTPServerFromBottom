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

  virtual void generateResponse(void) = 0;

 public:
  MethodHandler(void);
  MethodHandler(const Request& request_data, const Config& config_data);
  virtual ~MethodHandler(void);

  Response get_m_response_data(void);

 private:
  MethodHandler(const MethodHandler& obj);
  MethodHandler& operator=(MethodHandler const& obj);
};

// PostMethodHandler, DeleteMethodHandler도 만들기
class GetMethodHandler : public MethodHandler
{
 public:
  GetMethodHandler(const GetMethodHandler& obj);
  GetMethodHandler(const Request& request_data, const Config& config_data);
  virtual ~GetMethodHandler(void);
  GetMethodHandler& operator=(GetMethodHandler const& obj);

  void generateResponse(void);

  GetMethodHandler(void);
  // 나중에 지워야 함

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
