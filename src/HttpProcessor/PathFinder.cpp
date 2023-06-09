#include "PathFinder.hpp"

#include "Log.hpp"
// #include "./PathTest/PathFinder.hpp" // for test

#include <iostream>

PathFinder::PathFinder() {}

PathFinder::~PathFinder() {}

PathFinder::PathFinder(const PathFinder& origin){};

PathFinder& PathFinder::operator=(PathFinder const& origin) { return (*this); };

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
  // return (true);
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

void PathFinder::setIndex(std::string root, std::string file_name, std::string index_name,
                          Response& response_data)
{
  // Post method의 경우, 요청받은 file이 존재하지 않더라도
  // 요청받은 파일 name을 기록할 필요가 있다.
  response_data.file_name =  file_name;
  if (file_name == "")
  {
    response_data.file_exist = false;
  }
  else if (checkExist(root + file_name))
  {
    response_data.file_exist = true;
  }
  else
  {
    response_data.file_exist = false;
  }
  if (index_name == "")
  {
    response_data.index_exist = false;
  }
  else if (checkExist(root + index_name))
  {
    response_data.index_exist = true;
  }
  else
  {
    response_data.index_exist = false;
  }
}

void PathFinder::setAutoIndex(std::string auto_index, Response& response_data)
{
  if (auto_index == "on")
  {
    response_data.auto_index = true;
  }
}

bool PathFinder::setCgi(std::string locationBlock, t_server server_data,
                        Response& response_data)
{

  std::size_t pos_dot = locationBlock.find_last_of(".");
  if (pos_dot == std::string::npos) return (false); //uri 내부 .이 없는 경우
  std::size_t pos_slash = (locationBlock).rfind("/");
  // if (pos_slash == std::string::npos) return (false); //uri 내부 /가 없는 경우
  std::string location_key = locationBlock.substr(pos_slash + 1); //마지막 블럭 자르기
  pos_dot = location_key.find_last_of(".");
  if (pos_dot == std::string::npos) return (false); //마지막 블럭 내에 .이 없는 경우
  location_key = location_key.substr(pos_dot); //.이 있는 경우 확장자를 location_key로 설정
  if (server_data.locations.find(location_key) == server_data.locations.end())
    return false; //uri로 들어온 확장자가 config location에 존재하지 않을 때.
  t_location current_location = server_data.locations.find(location_key)->second;
  if (current_location.ourcgi_pass == "" || current_location.ourcgi_index == "")
    return false; //블록에 ourcgi_pass와 ourcgi_index가 모두 있어야 cgi
  if (!checkExist(current_location.ourcgi_pass) || is_directory(current_location.ourcgi_pass))
    throw BAD_GATEWAY_502; //ourcgi_pass에 입력된 파일이 없는 경우 혹은 넘어온 경로가 디렉토리일 경우 error
  else
  { //cgi 일때 response 값 설정
    response_data.cgi_flag = true;
    response_data.cgi_bin_path = current_location.ourcgi_pass;
    response_data.root_path = current_location.root;
    response_data.uploaded_path =
        current_location.uploaded_path;
    setIndex(current_location.root + "/", current_location.ourcgi_index, current_location.index,
             response_data);
    setMethod(current_location.accepted_method, response_data);
    return true;
  }
  // if (locationBlock.substr(locationBlock.find_last_of(".")) == ".py")
  // {
  //   response_data.cgi_flag = true;
  //   t_location current_location = server_data.locations.find(".py")->second;
  //   response_data.cgi_bin_path = current_location.ourcgi_pass;
  //   response_data.root_path = current_location.root;
  //   response_data.uploaded_path =
  //       current_location.uploaded_path;  // 경로 존재하는지
  //   setIndex(current_location.root + "/", current_location.ourcgi_index, current_location.index,
  //            response_data);
  //   setMethod(current_location.accepted_method, response_data);
  //   return true;
  // }
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

void PathFinder::test_print_location(t_location& c)
{
  LOG_DEBUG("🧪 test_print_location 🧪");
  LOG_DEBUG("language: %s", c.language.c_str());
  LOG_DEBUG("root: %s", c.root.c_str());
  LOG_DEBUG("auto_index: %d", c.auto_index.c_str());
  LOG_DEBUG("index: %s", c.index.c_str());
  LOG_DEBUG("ourcgi_pass: %s", c.ourcgi_pass.c_str());
  LOG_DEBUG("ourcgi_index: %s", c.ourcgi_index.c_str());
  LOG_DEBUG("uploaded_path: %s", c.uploaded_path.c_str());
  LOG_DEBUG("accepted_method: %s", c.accepted_method.c_str());
  LOG_DEBUG("🧪 test_print_location 🧪");
}

void PathFinder::test_print_basics(Response& c)
{
  LOG_DEBUG("🧪 test_print_basics 🧪");
  LOG_DEBUG("accepted_method: %s", c.accepted_method.c_str());
  LOG_DEBUG("auto_index: %d", c.auto_index);
  LOG_DEBUG("file_exist: %d", c.file_exist);
  LOG_DEBUG("file_name: %s", c.file_name.c_str());
  LOG_DEBUG("path_exist: %d", c.path_exist);
  LOG_DEBUG("file_path: %s", c.file_path.c_str());
  LOG_DEBUG("cgi_flag: %d", c.cgi_flag);
  LOG_DEBUG("cgi_bin_path: %s", c.cgi_bin_path.c_str());
  LOG_DEBUG("uploaded_path: %s", c.uploaded_path.c_str());
  LOG_DEBUG("🧪 test_print_basics 🧪");
}

void PathFinder::setRootPath(std::string root_path, Response& response_data)
{
  response_data.root_path = root_path;
}

void PathFinder::setBasic(std::string method, std::string root,
                          std::string file_name, std::string index_name, 
                          std::string auto_index,
                          std::string upload, std::string redirection,
                          std::string root_path, Response& response_data)
{
  LOG_DEBUG("Default server block (root: %s, file: %s, index: %s)", root.c_str(),
            file_name.c_str(), index_name.c_str());
  setMethod(method, response_data);
  setRoot(root, response_data);
  setIndex(root, file_name, index_name, response_data);
  setUpload(upload, response_data);
  setAutoIndex(auto_index, response_data);
  setRedirection(redirection, response_data);
  setRootPath(root_path, response_data);
  LOG_DEBUG("Default server block_exist (root: %d, file: %d, index: %d)", response_data.path_exist,
            response_data.file_exist, response_data.index_exist);
}

PathFinder::PathFinder(Request& request_data, t_server& server_data,
                       Response& response_data)
{
  std::string locationBlock;
  t_location current_location;

  locationBlock = request_data.uri;
  if (locationBlock.find("//") != std::string::npos) throw NOT_FOUND_404;

  std::map<std::string, t_location>::iterator temp_location;
  if ((locationBlock) == "/" || (locationBlock) == "")  // default block
  {
    current_location = server_data.locations.find("/")->second;
    setBasic(current_location.accepted_method, current_location.root + "/",
             "" , current_location.index, current_location.auto_index,
             current_location.uploaded_path, current_location.redirection,
             current_location.root, response_data);
    return;
  }
  if (setCgi((locationBlock), server_data, response_data))
  {
    LOG_DEBUG("after setCgi m_response_data (cgi_f: %d, bin: %s, index: %s)", response_data.cgi_flag,
            response_data.cgi_bin_path.c_str(), response_data.index_name.c_str());
    return;
  }
  LOG_DEBUG("after setCgi m_response_data (cgi_f: %d, bin: %s, index: %s)", response_data.cgi_flag,
            response_data.cgi_bin_path.c_str(), response_data.index_name.c_str());
  std::size_t pos_last = (locationBlock).rfind("/");
  if (pos_last == 0)  // '/a'처럼 location 블록이름만 들어온 경우
  {
    temp_location = server_data.locations.find(locationBlock);
    if (temp_location == server_data.locations.end())
    {
      current_location = server_data.locations.find("/")->second;
      if (checkExist(current_location.root + locationBlock))
      {  // '/' 기본 블럭 뒤 파일 이름 or 디렉토리 이름 허용 -> default 위치
         // auto 인덱스 하려면 꼭 필요
        if (is_directory(current_location.root + locationBlock))
        {
          setBasic(current_location.accepted_method,
                   current_location.root + locationBlock + "/",
                   "" , current_location.index, current_location.auto_index,
                   current_location.uploaded_path, current_location.redirection,
                   current_location.root, response_data);
        }
        else
        {
          setBasic(current_location.accepted_method,
                   current_location.root + "/", locationBlock.substr(1),
                   current_location.index,
                   current_location.auto_index, current_location.uploaded_path,
                   current_location.redirection, current_location.root,
                   response_data);
        }
      }
      else
      {
        throw NOT_FOUND_404;
        // 들어온 블록이름이 location에 존재하지 않음.
      }
    }
    else
    {
      current_location = temp_location->second;
      setBasic(current_location.accepted_method, current_location.root + "/",
               "", current_location.index, current_location.auto_index,
               current_location.uploaded_path, current_location.redirection,
               current_location.root, response_data);
    }
  }
  else
  {  // "/block_name/b/c/d", "/??(location에 없음)/b/c/d"
     //"a/b/c/d(디렉토리)", "/a/b/c/d/e(파일)"
     // '/'로 끝나는 경우와 "/파일이름.txt(경로없이)"인 경우는 고려하지 않음.
     //   std::string path = "/a/b/c/d";
     // std::cout << "in this block" << std::endl;
    std::string location_key =
        (locationBlock).substr(0, (locationBlock).find("/", 1));
    temp_location = server_data.locations.find(location_key);
    if (temp_location == server_data.locations.end())
    {  // "/??(location에 없음)/b/c/d" 경우 + "/(기본디렉토리)/(그 안의
       // 디렉토리)/그 안의 파일"
      current_location = server_data.locations.find("/")->second;
      if (checkExist(current_location.root + locationBlock))
      {  //  기본 블럭 뒤 파일 이름 or 디렉토리 이름 허용 -> default 위치 auto
         //  인덱스 하려면 꼭 필요
        std::string rest_of_uri =
            (locationBlock).substr((locationBlock).find("/"));
        std::cout << rest_of_uri << std::endl;
        std::string entire_path = current_location.root + rest_of_uri;
        pos_last = entire_path.rfind("/");
        if (!is_directory(current_location.root + locationBlock))
        {  // 파일로 끝나는 경로가 온 경우
          setBasic(current_location.accepted_method,
                   entire_path.substr(0, pos_last + 1),
                   entire_path.substr(pos_last + 1),
                   current_location.index,
                   current_location.auto_index, current_location.uploaded_path,
                   current_location.redirection, current_location.root,
                   response_data);
          return;
        }
        // 디렉토리로 끝나는 경우가 온 경우
        if (locationBlock[locationBlock.length() - 1] != '/')
          locationBlock += "/"; //디렉토리 뒤 '/'
        setBasic(current_location.accepted_method,
                 current_location.root + locationBlock,
                 "", current_location.index,
                 current_location.auto_index, current_location.uploaded_path,
                 current_location.redirection, current_location.root,
                 response_data);
      }
      else
      {  // 존재하지 않는 블럭 && 디폴트 폴더 내부 파일 or 디렉토리도 아님
        setBasic(current_location.accepted_method, "", "", "",
                 current_location.auto_index, current_location.uploaded_path,
                 current_location.redirection, current_location.root,
                 response_data);
      }
      return;
    }
    current_location = temp_location->second;
    std::string rest_of_uri =
        (locationBlock).substr((locationBlock).find("/", 1));
    std::string entire_path = current_location.root + rest_of_uri;
    pos_last = entire_path.rfind("/");
    if (is_directory(entire_path))  //"a/b/c/d(존재하는 디렉토리)"
    {
      if (entire_path[entire_path.length() - 1] == '/')
          entire_path.pop_back(); //디렉토리뒤 '/'
      setBasic(current_location.accepted_method, entire_path + "/",
               "", current_location.index, current_location.auto_index,
               current_location.uploaded_path, current_location.redirection,
               current_location.root, response_data);
    }
    else
    {  //"/a/b/c/d/e(파일)" 경우
      LOG_DEBUG("pos_last: %d, entire_path: %s", pos_last, entire_path.c_str());
      setBasic(current_location.accepted_method,
               entire_path.substr(0, pos_last + 1),
               entire_path.substr(pos_last + 1), current_location.index,
               current_location.auto_index,
               current_location.uploaded_path, current_location.redirection,
               current_location.root, response_data);
    }
  }
}