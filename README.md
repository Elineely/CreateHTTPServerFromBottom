## Desc

- HttpProcessor가 methodHandler를 호출하기 전 단계인 1차 가공 중 "클라이언트에게 제공할 리소스"의 실제 path와 제공할 파일이름등을 반환하는 함수를 만듭니다.


## Detail

- 제공할 리소스(동적, 정적 포함)의 실제 위치 후보들을 config 구조체 및 requset 구조체 내부 멤버 변수들을 통해 조합하고, 우선순위에 맞는 리소스 위치를 string 형식으로 제공합니다.
만일 웹서버의 configuration file에 기술된 파일을 모두 찾을 수 없으면 빈 string을 반환하게 됩니다.

- PathFinder 객체는 HttpProcessor에 존재하며, request_data 내 uri, t_server 내에 포함된 location data들을 조합하여, methodHandler에서 필요로 하는 file_path 정보 및 다양한 플래그를 제공합니다.
```
class HttpProcessor
{
  HttpProcessor(Request& request_data, t_server& server_data);
 (...)
  
 private:
  HttpProcessor(void);
  Request m_request_data;
  Response m_response_data;
  t_server m_server_data;
  PathFinder m_path_finder(Request request_data, t_server server_data, Response& response_data);
(...)
}
```
- PathFinder를 통해 변경될 수 있는 Response 내 변수들은 다음과 같습니다.
- body, response_message, pipe_fd를 제외한 std::string 및 bool타입 변수들입니다.
```
struct Response
{
  std::string accepted_method;
  bool redirection_exist;
  std::string rediretion_location;
  bool auto_index;
  std::string file_name;
  bool file_exist;
  std::string file_path;
  bool path_exist;
  StatusCode status_code;
  bool cgi_flag;
  std::string cgi_bin_path;
  std::string uploaded_path;
};
```
- 
```
void PathFinder::setRoot(std::string root, Response& response_data)
{
  if (checkExist(root))
  {
    response_data.file_path = root;
    response_data.path_exist = true;
  }
}
```

## Todo

- [x] 파일 존재 여부 확인 기능 구현하기
- [x] 동적 컨텐츠의 리소스가 없을 경우 m_file_path 값 설정하기
- [x] response 객체 내 m_file_path 구하는 함수 만들기
- [x] pathFinder 기능 테스트 하기

## Etc
#62 이슈에 기록된 configuration 룰을 따르며,
현 파트에서는 유효성을 다루지 않습니다.