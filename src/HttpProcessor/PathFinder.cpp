#include "PathFinder.hpp"

PathFinder::PathFinder() {}

PathFinder::~PathFinder() {}

bool PathFinder::is_directory(const std::string& path)
{
  DIR* dir = opendir(path.c_str());
  if (dir)
  {
    closedir(dir);
    return true;
  }
  return false;
}

bool PathFinder::checkExist(const std::string& path_or_file)
{
  return (access(path_or_file.c_str(), F_OK) == 0);
}

void PathFinder::setMethod(std::string method, Response& response_data)
{
  response_data.accepted_method = method;
}

void PathFinder::setUpload(std::string upload, Response& response_data)
{
  response_data.uploaded_path = upload;
}

void PathFinder::setRoot(std::string root, Response& response_data)
{
  if (checkExist(root))
  {
    response_data.file_path = root;
    response_data.path_exist = true;
  }
}

void PathFinder::setIndex(std::string root, std::string file_name,
                          std::string index_name, Response& response_data)
{
  // Post method의 경우, 요청받은 file이 존재하지 않더라도
  // 요청받은 파일 name을 기록할 필요가 있다.
  response_data.file_name = file_name;
  response_data.index_name = index_name;
  if (file_name == "")
    response_data.file_exist = false;
  else if (checkExist(root + file_name))
    response_data.file_exist = true;
  else
    response_data.file_exist = false;
  if (index_name == "")
    response_data.index_exist = false;
  else if (checkExist(root + index_name))
  {
    response_data.index_exist = true;
  }
  else
    response_data.index_exist = false;
}

void PathFinder::setAutoIndex(std::string auto_index, Response& response_data)
{
  if (auto_index == "on")
  {
    response_data.auto_index = true;
  }
}

bool PathFinder::setCgi(std::string locationBlock, t_server server_data,
                        Response& response_data, Request& request_data)
{
  std::size_t pos_dot = locationBlock.find_last_of(".");
  if (pos_dot == std::string::npos) return (false);  // uri 내부 .이 없는 경우
  std::size_t pos_slash = (locationBlock).rfind("/");
  // if (pos_slash == std::string::npos) return (false); //uri 내부 /가 없는
  // 경우
  std::string location_key =
      locationBlock.substr(pos_slash + 1);  // 마지막 블럭 자르기
  pos_dot = location_key.find_last_of(".");
  if (pos_dot == std::string::npos)
    return (false);  // 마지막 블럭 내에 .이 없는 경우
  location_key = location_key.substr(pos_dot);  
  //.이 있는 경우 확장자를 location_key로 설정
  if (server_data.locations.find(location_key) == server_data.locations.end())
    return false;  // uri로 들어온 확장자가 config location에 존재하지 않을 때.
  t_location current_location =
      server_data.locations.find(location_key)->second;
  if (current_location.ourcgi_pass == "" )
    return false;  // 블록에 ourcgi_pass와 ourcgi_index가 모두 있어야 cgi
  if (!checkExist(current_location.ourcgi_pass) ||
      is_directory(current_location.ourcgi_pass) || current_location.ourcgi_index == "")
    throw BAD_GATEWAY_502;  // ourcgi_pass에 입력된 파일이 없는 경우 혹은 넘어온
                            // 경로가 디렉토리일 경우 error
  else
  {  // cgi 일때 response 값 설정
    setMaxSize(request_data, current_location.max_body_size);
    response_data.cgi_flag = true;
    response_data.cgi_bin_path = current_location.ourcgi_pass;
    response_data.root_path = current_location.root;
    response_data.uploaded_path = current_location.uploaded_path;
    setIndex(current_location.root + "/", current_location.ourcgi_index,
             current_location.index, response_data);
    setMethod(current_location.accepted_method, response_data);
    return true;
  }
}

void PathFinder::setRedirection(std::string redirection,
                                Response& response_data)
{
  if (redirection != "")
  {
    response_data.rediretion_location = redirection;
    response_data.redirection_exist = true;
  }
}

void PathFinder::setRootPath(std::string root_path, Response& response_data)
{
  response_data.root_path = root_path;
}

void PathFinder::setBasic(std::string method, std::string root,
                          std::string file_name, std::string index_name,
                          std::string auto_index, std::string upload,
                          std::string redirection, std::string root_path,
                          Response& response_data)
{
  setMethod(method, response_data);
  setRoot(root, response_data);
  setIndex(root, file_name, index_name, response_data);
  setUpload(upload, response_data);
  setAutoIndex(auto_index, response_data);
  setRedirection(redirection, response_data);
  setRootPath(root_path, response_data);
}

void PathFinder::setMaxSize(Request request_data, std::string max_body_size)
{
  checkMaxSize(request_data, std::atol(max_body_size.c_str()));
}

void PathFinder::checkMaxSize(Request request_data, long max_body_size)
{

  if (static_cast<long>(request_data.body.size()) > max_body_size)
  {
    throw PAYLOAD_TOO_LARGE_413;
  }
}

// functions for abstraction

bool PathFinder::isRootBlock(std::string locationBlock, t_server& server_data,
                             Response& response_data, Request& request_data)
{
  t_location current_location;

  if ((locationBlock) == "/" || (locationBlock) == "")  // default block
  {
    std::map<std::string, t_location>::iterator temp = server_data.locations.find("/");
    if (temp == server_data.locations.end()) throw NOT_FOUND_404;
    current_location = server_data.locations.find("/")->second;
    setMaxSize(request_data, current_location.max_body_size);
    setBasic(current_location.accepted_method, current_location.root + "/", "",
             current_location.index, current_location.auto_index,
             current_location.uploaded_path, current_location.redirection,
             current_location.root, response_data);
    return true;
  }
  return false;
}

bool PathFinder::isCgiBlock(std::string locationBlock, t_server& server_data,
                            Response& response_data, Request& request_data)
{
  if (setCgi((locationBlock), server_data, response_data, request_data))
  {
    return true;
  }
  return false;
}

void PathFinder::oneSlashInUri(t_server& server_data, std::string locationBlock,
                               Response& response_data, Request& request_data)
{
  t_location current_location;
  std::map<std::string, t_location>::iterator temp_location;

  temp_location = server_data.locations.find(locationBlock);
  if (temp_location == server_data.locations.end())
  {
    std::map<std::string, t_location>::iterator temp = server_data.locations.find("/");
    if (temp == server_data.locations.end()) throw NOT_FOUND_404;
    current_location = server_data.locations.find("/")->second;
    if (checkExist(current_location.root + locationBlock))
    {  // '/' 기본 블럭 뒤 파일 이름 or 디렉토리 이름 허용 -> default 위치
       // auto 인덱스 하려면 꼭 필요
      if (isEndWithDirectory(current_location.root + locationBlock, request_data, current_location, response_data))
        return;
      else
      {
        setMaxSize(request_data, current_location.max_body_size);
        setBasic(current_location.accepted_method, current_location.root + "/",
                 locationBlock.substr(1), current_location.index,
                 current_location.auto_index, current_location.uploaded_path,
                 current_location.redirection, current_location.root,
                 response_data);
      }
    }
    else
    {
      // 들어온 블록이름이 location에 존재하지 않음.
      throw NOT_FOUND_404;
    }
  }
  else
  {
    // location 이름이 들어옴
    current_location = temp_location->second;
    setMaxSize(request_data, current_location.max_body_size);
    setBasic(current_location.accepted_method, current_location.root + "/", "",
             current_location.index, current_location.auto_index,
             current_location.uploaded_path, current_location.redirection,
             current_location.root, response_data);
  }
}

// bool PathFinder::isEndWithDirectory(std::string locationBlock,
//                                     t_location current_location,
//                                     Response& response_data,
//                                     Request& request_data)
// {
//   // 디렉토리로 끝나는 경우가 온 경우
//   if (is_directory(current_location.root +
//                    locationBlock))  //"a/b/c/d(존재하는 디렉토리)"
//   {
//     if (locationBlock[locationBlock.length() - 1] != '/')
//       locationBlock += "/";  // 디렉토리 뒤 '/'
//     setMaxSize(request_data, current_location.max_body_size);
//     setBasic(current_location.accepted_method,
//              current_location.root + locationBlock, "", current_location.index,
//              current_location.auto_index, current_location.uploaded_path,
//              current_location.redirection, current_location.root,
//              response_data);
//     return true;
//   }
//   return false;
// }

bool PathFinder::isEndWithDirectory(std::string entire_path,
                                         Request& request_data,
                                         t_location current_location,
                                         Response& response_data)
{
  if (is_directory(entire_path))  //"a/b/c/d(존재하는 디렉토리)"
  {
    if (entire_path[entire_path.length() - 1] == '/')
      entire_path.pop_back();  // 디렉토리뒤 '/'
    setMaxSize(request_data, current_location.max_body_size);
    setBasic(current_location.accepted_method, entire_path + "/", "",
             current_location.index, current_location.auto_index,
             current_location.uploaded_path, current_location.redirection,
             current_location.root, response_data);
    return true;
  }
  return false;
}

void PathFinder::isEndWithFileName(std::string entire_path,
                                   Request& request_data,
                                   t_location current_location,
                                   Response& response_data)
{
  size_t pos_last = entire_path.rfind("/");
  //"/a/b/c/d/e(파일)" 경우
  setMaxSize(request_data, current_location.max_body_size);
  setBasic(current_location.accepted_method,
           entire_path.substr(0, pos_last + 1),
           entire_path.substr(pos_last + 1), current_location.index,
           current_location.auto_index, current_location.uploaded_path,
           current_location.redirection, current_location.root, response_data);
}


bool PathFinder::firstBlockIsNotLocation(t_server& server_data,
                                         std::string location_key,
                                         t_location current_location,
                                         std::string locationBlock,
                                         Response& response_data,
                                         Request& request_data)
{
  std::map<std::string, t_location>::iterator temp_location;

  temp_location = server_data.locations.find(location_key);
  if (temp_location == server_data.locations.end())
  {  //(기본디렉토리)/(내부 디렉토리)/내부 파일"
    std::map<std::string, t_location>::iterator temp = server_data.locations.find("/");
    if (temp == server_data.locations.end()) throw NOT_FOUND_404;
    current_location = server_data.locations.find("/")->second;
    if (checkExist(current_location.root + location_key))
    {  //  기본 블럭 뒤 파일 이름 or 디렉토리 이름 허용 -> default 위치 auto
       //  인덱스 하려면 꼭 필요
      // std::string entire_path = current_location.root + locationBlock;
      if (isEndWithDirectory(current_location.root + locationBlock, request_data, current_location, response_data))
        return true;
      else
        isEndWithFileName(current_location.root + locationBlock, request_data, current_location,
                          response_data);
    }
    else
    {  // 존재하지 않는 블럭 && (디폴트 폴더 내부 파일아님 && 디렉토리도 아님)
      //"/??(location에 없음)/b/c/d" 경우
      throw NOT_FOUND_404;
    }
    return true;
  }
  return false;
}

void PathFinder::firstBlockIsLocation(std::string location_key,
                                      t_server& server_data,
                                      std::string locationBlock,
                                      Request& request_data,
                                      Response& response_data)
{
  std::map<std::string, t_location>::iterator temp_location =
      server_data.locations.find(location_key);
  t_location current_location = temp_location->second;
  std::string rest_of_uri =
      (locationBlock).substr((locationBlock).find("/", 1));

  if (isEndWithDirectory(current_location.root + rest_of_uri, request_data, current_location,
                              response_data))
    return;
  else
    isEndWithFileName(current_location.root + rest_of_uri, request_data, current_location,
                      response_data);
}

void PathFinder::manySlashesInUri(std::string locationBlock,
                                  t_server& server_data,
                                  Response& response_data,
                                  Request& request_data)
{
  // "/block_name/b/c/d", "/??(location에 없음)/b/c/d"
  //"a/b/c/d(디렉토리)", "/a/b/c/d/e(파일)"
  t_location current_location;
  std::map<std::string, t_location>::iterator temp_location;

  std::string location_key =
      (locationBlock).substr(0, (locationBlock).find("/", 1));
  if (firstBlockIsNotLocation(server_data, location_key, current_location,
                              locationBlock, response_data, request_data))
    return;
  else
    firstBlockIsLocation(location_key, server_data, locationBlock, request_data,
                         response_data);
}

PathFinder::PathFinder(Request& request_data, t_server& server_data,
                       Response& response_data)
{
  std::string locationBlock = request_data.uri;

  if (locationBlock.find("//") != std::string::npos) throw NOT_FOUND_404;

  if (isRootBlock(locationBlock, server_data, response_data, request_data))
    return;
  if (isCgiBlock(locationBlock, server_data, response_data, request_data)) return;

  std::size_t pos_last = (locationBlock).rfind("/");
  if (pos_last == 0)
    oneSlashInUri(server_data, locationBlock, response_data, request_data);
  else
    manySlashesInUri(locationBlock, server_data, response_data, request_data);
}
