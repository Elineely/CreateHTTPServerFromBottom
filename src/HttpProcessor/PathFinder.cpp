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
  //   else
  //   {
  //     response_data.path_exist = false;
  //   } //넣을까말까 고민중.. 초기화때 false가 있으면 필요없을 듯
}

void PathFinder::setIndex(std::string root, std::string index,
                          Response& response_data)
{
  // Post method의 경우, 요청받은 file이 존재하지 않더라도
  // 요청받은 파일 name을 기록할 필요가 있다.
  response_data.file_name =  index;
  if (checkExist(root + index))
  {
    response_data.file_exist = true;
  }
  else
  {
    response_data.file_exist = false;
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
  std::size_t pos_last = locationBlock.find_last_of(".");
  if (pos_last == std::string::npos) return (false);
  if (locationBlock.substr(locationBlock.find_last_of(".")) == ".py")
  {
    response_data.cgi_flag = true;
    t_location current_location = server_data.locations.find(".py")->second;
    response_data.cgi_bin_path = current_location.ourcgi_pass;
    response_data.root_path = current_location.root;
    response_data.uploaded_path =
        current_location.uploaded_path;  // 경로 존재하는지
    setIndex(current_location.root + "/", current_location.ourcgi_index,
             response_data);
    setMethod(current_location.accepted_method, response_data);
    return true;
  }
  return false;
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
                          std::string index, std::string auto_index,
                          std::string upload, std::string redirection,
                          std::string root_path, Response& response_data)
{
  LOG_DEBUG("Default server block (root: %s, index: %s)", root.c_str(),
            index.c_str());
  setMethod(method, response_data);
  setRoot(root, response_data);
  setIndex(root, index, response_data);
  setUpload(upload, response_data);
  setAutoIndex(auto_index, response_data);
  setRedirection(redirection, response_data);
  setRootPath(root_path, response_data);
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
             current_location.index, current_location.auto_index,
             current_location.uploaded_path, current_location.redirection,
             current_location.root, response_data);
    if (response_data.auto_index == true)
    {
      response_data.file_name = "";
      response_data.file_exist = false;
    }
    return;
  }
  if (setCgi((locationBlock), server_data, response_data))
  {
    return;
  }
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
                   current_location.index, current_location.auto_index,
                   current_location.uploaded_path, current_location.redirection,
                   current_location.root, response_data);
          if (response_data.auto_index == true)
          {
            response_data.file_name = "";
            response_data.file_exist = false;
          }
        }
        else
        {
          setBasic(current_location.accepted_method,
                   current_location.root + "/", locationBlock.substr(1),
                   current_location.auto_index, current_location.uploaded_path,
                   current_location.redirection, current_location.root,
                   response_data);
        }
      }
      else
      {
        // 들어온 블록이름이 location에 존재하지 않음.
        response_data.path_exist = false;
        response_data.file_exist = false;
        response_data.auto_index = false;
      }
    }
    else
    {
      current_location = temp_location->second;
      setBasic(current_location.accepted_method, current_location.root + "/",
               current_location.index, current_location.auto_index,
               current_location.uploaded_path, current_location.redirection,
               current_location.root, response_data);
      if (response_data.auto_index == true)
      {
        response_data.file_name = "";
        response_data.file_exist = false;
      }
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
                   current_location.auto_index, current_location.uploaded_path,
                   current_location.redirection, current_location.root,
                   response_data);
          return;
        }
        // 디렉토리로 끝나는 경우가 온 경우
        setBasic(current_location.accepted_method,
                 current_location.root + locationBlock, current_location.index,
                 current_location.auto_index, current_location.uploaded_path,
                 current_location.redirection, current_location.root,
                 response_data);
        if (response_data.auto_index == true)
        {
          response_data.file_name = "";
          response_data.file_exist = false;
        }
      }
      else
      {  // 존재하지 않는 블럭 && 디폴트 폴더 내부 파일 or 디렉토리도 아님
        setBasic(current_location.accepted_method, "", "",
                 current_location.auto_index, current_location.uploaded_path,
                 current_location.redirection, current_location.root,
                 response_data);
        response_data.path_exist = false;
        response_data.file_exist = false;
        response_data.auto_index = false;
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
      setBasic(current_location.accepted_method, entire_path + "/",
               current_location.index, current_location.auto_index,
               current_location.uploaded_path, current_location.redirection,
               current_location.root, response_data);
      if (response_data.auto_index == true)
      {
        response_data.file_name = "";
        response_data.file_exist = false;
      }
    }
    else
    {  //"/a/b/c/d/e(파일)" 경우
      LOG_DEBUG("pos_last: %d, entire_path: %s", pos_last, entire_path.c_str());
      setBasic(current_location.accepted_method,
               entire_path.substr(0, pos_last + 1),
               entire_path.substr(pos_last + 1), current_location.auto_index,
               current_location.uploaded_path, current_location.redirection,
               current_location.root, response_data);
    }
  }
}