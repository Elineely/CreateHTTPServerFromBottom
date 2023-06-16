#include "MethodHandler.hpp"

#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>

#include <algorithm>
#include <cstdio>
#include <ctime>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

std::string MethodHandler::generateDate(const std::time_t& timestamp)
{
  char buffer[25];
  std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S",
                std::localtime(&timestamp));
  return buffer;
}
std::string MethodHandler::generateSize(const long long int& fileSize)
{
  std::stringstream ss;
  ss << fileSize << " Byte";
  return ss.str();
}
bool MethodHandler::fileInfoCompare(const FileInfo& fileInfo1,
                                    const FileInfo& fileInfo2)
{
  return (fileInfo1.name < fileInfo2.name);
}
void MethodHandler::autoIndexToBody(std::string target_directory)
{
  if (m_response_data.file_path !=
      m_response_data.file_path + m_response_data.file_name)
    throw NOT_FOUND_404;
  size_t pos = m_request_data.uri.find("//");
  if (pos != std::string::npos)
    m_request_data.uri.erase(m_request_data.uri.end() - 1);
  std::vector<FileInfo> fileList;
  DIR* dir;
  struct dirent* entry;
  dir = opendir(target_directory.c_str());
  if (dir == NULL) throw INTERNAL_SERVER_ERROR_500;
  entry = readdir(dir);
  while (entry != NULL)
  {
    std::string file_name = entry->d_name;
    struct stat file_stat;
    std::string file_path = target_directory + file_name;
    if (stat(file_path.c_str(), &file_stat) != -1)
    {
      FileInfo fileData;
      if (S_ISDIR(file_stat.st_mode))
        fileData.is_dir = "/";
      else
        fileData.is_dir = "";
      fileData.name = file_name;
      fileData.date = file_stat.st_mtime;
      fileData.size = file_stat.st_size;
      fileList.push_back(fileData);
    }
    entry = readdir(dir);
  }
  closedir(dir);
  std::sort(fileList.begin(), fileList.end(), fileInfoCompare);
  fileList.erase(fileList.begin());
  if (target_directory == (m_response_data.root_path + "/"))
    fileList.erase(fileList.begin());
  std::stringstream autoindex;
  autoindex << "<html>\n"
            << "\t<head>\n"
            << "\t\t<style>\n"
            << "\t\t\ttable { width: 70%; }\n"
            << "\t\t\tth, td { text-align: left; }\n"
            << "\t\t</style>\n"
            << "\t</head>\n"
            << "\t<body>\n"
            << "\t\t<h1>Index of " << m_request_data.uri << "</h1>\n";
  autoindex
      << "\t\t<table>\n"
      << "\t\t\t<tr><th>Name</th><th>Last Modified</th><th>Size</th></tr>\n";
  if ((m_request_data.uri != "/") &&
      (m_request_data.uri[m_request_data.uri.length() - 1] == '/'))
    m_request_data.uri.pop_back();
  for (std::vector<FileInfo>::iterator it = fileList.begin();
       it != fileList.end(); ++it)
  {
    std::string date = generateDate((*it).date);
    std::string size = generateSize((*it).size);
    autoindex << "\t\t\t<tr><td><a href=\""
              << (m_request_data.uri == "/" ? "" : m_request_data.uri) << "/"
              << (*it).name << "\">" << (*it).name << (*it).is_dir
              << "</a></td><td>" << date << "</td><td>" << size
              << "</td></tr>\n";
  }
  autoindex << "\t\t</table>\n"
            << "\t</body>\n"
            << "</html>";
  std::string autoindex_str;
  autoindex_str = autoindex.str();
  m_response_data.body.insert(m_response_data.body.end(), autoindex_str.begin(),
                              autoindex_str.end());
}

void MethodHandler::fileToBody(std::string target_file)
{
  std::ifstream file(target_file, std::ios::binary);
  if (!file.is_open()) throw INTERNAL_SERVER_ERROR_500;

  // Determine the file size, resize the vector
  // offset 을 마지막으로 밀기
  file.seekg(0, std::ios::end);
  // 파일 사이즈 구하기
  std::streampos file_size = file.tellg();
  // 다시 돌리기
  file.seekg(0, std::ios::beg);
  std::vector<char> buffer(file_size);

  // Read the file contents into the vector
  file.read(&buffer[0], file_size);
  file.close();

  // contents of buffer into body
  m_response_data.body = buffer;
}

// MethodHandler
MethodHandler::MethodHandler(const MethodHandler& obj)
    : m_request_data(obj.m_request_data), m_response_data(obj.m_response_data)
{
}
MethodHandler::MethodHandler(Request& request_data, Response& response_data)
    : m_request_data(request_data), m_response_data(response_data)
{
}
MethodHandler::~MethodHandler(void) {}

// GetMethodHandler
GetMethodHandler::GetMethodHandler(const GetMethodHandler& obj)
    : MethodHandler(obj)
{
}
GetMethodHandler::GetMethodHandler(Request& request_data,
                                   Response& response_data)
    : MethodHandler(request_data, response_data)
{
}
GetMethodHandler::~GetMethodHandler(void) {}
void GetMethodHandler::methodRun()
{
  if (m_response_data.path_exist == false) throw NOT_FOUND_404;
  if (m_response_data.file_exist == true)
    fileToBody(m_response_data.file_path + m_response_data.file_name);
  else
  {
    if (m_response_data.file_name != "")
      throw NOT_FOUND_404;
    else if (m_response_data.index_exist == true)
      fileToBody(m_response_data.file_path + m_response_data.index_name);
    else if (m_response_data.auto_index == true)
      autoIndexToBody(m_response_data.file_path);
    else
      throw NOT_FOUND_404;
  }
}

// PostMethodHandler
PostMethodHandler::PostMethodHandler(const PostMethodHandler& obj)
    : MethodHandler(obj)
{
}
PostMethodHandler::PostMethodHandler(Request& request_data,
                                     Response& response_data)
    : MethodHandler(request_data, response_data)
{
}
PostMethodHandler::~PostMethodHandler(void) {}
void PostMethodHandler::methodRun()
{
  if (m_response_data.path_exist == false)
  {
    throw BAD_REQUEST_400;
  }
  if (m_response_data.file_exist == false && m_response_data.file_name == "")
  {
    m_response_data.file_name = "default.temp";
    if (access(m_response_data.file_name.c_str(), F_OK) == 0)
      m_response_data.file_exist = true;
  }
  std::string target_file(m_response_data.file_path +
                          m_response_data.file_name);
  // ft_delete(the target file
  if (m_response_data.file_exist == true)
  {
    // error deleting file
    if (std::remove(&target_file[0]) != 0)
    {
      throw INTERNAL_SERVER_ERROR_500;
    }
  }
  if (m_request_data.body.size() == 0)
  {
    return;
  }
  std::ofstream new_file_stream(target_file, std::ios::binary);
  if (!new_file_stream)
  {
    throw INTERNAL_SERVER_ERROR_500;
  }
  new_file_stream.write(&m_request_data.body[0], m_request_data.body.size());
  new_file_stream.close();
}

// DeleteMethodHandler
DeleteMethodHandler::DeleteMethodHandler(const DeleteMethodHandler& obj)
    : MethodHandler(obj)
{
}
DeleteMethodHandler::DeleteMethodHandler(Request& request_data,
                                         Response& response_data)
    : MethodHandler(request_data, response_data)
{
}
DeleteMethodHandler::~DeleteMethodHandler(void) {}
void DeleteMethodHandler::methodRun()
{
  if (m_response_data.path_exist == false)
  {
    throw BAD_REQUEST_400;
  }
  std::string target_file(m_response_data.file_path +
                          m_response_data.file_name);
  if (m_response_data.file_exist == true)
    // error deleting file
    if (std::remove(&target_file[0]) != 0) throw INTERNAL_SERVER_ERROR_500;
}

// PutMethodHandler
PutMethodHandler::PutMethodHandler(const PutMethodHandler& obj)
    : MethodHandler(obj)
{
}
PutMethodHandler::PutMethodHandler(Request& request_data,
                                   Response& response_data)
    : MethodHandler(request_data, response_data)
{
}
PutMethodHandler::~PutMethodHandler(void) {}
void PutMethodHandler::methodRun()
{
  if (m_response_data.path_exist == false)
  {
    throw BAD_REQUEST_400;
  }
  if (m_response_data.file_exist == false && m_response_data.file_name == "")
  {
    m_response_data.file_name = "default.temp";
    if (access(m_response_data.file_name.c_str(), F_OK) == 0)
      m_response_data.file_exist = true;
  }
  std::string target_file(m_response_data.file_path +
                          m_response_data.file_name);

  if (m_response_data.file_exist == false)
  {
    std::ofstream new_file_stream(target_file,
                                  std::ios::app | std::ios::binary);
    if (!new_file_stream) throw INTERNAL_SERVER_ERROR_500;
    new_file_stream.write(&m_request_data.body[0], m_request_data.body.size());
    new_file_stream.close();
  }
  else
  {
    std::ofstream append_stream(target_file, std::ios::app);
    std::stringstream content_stream;
    if (!append_stream) throw INTERNAL_SERVER_ERROR_500;
    content_stream.write(&m_request_data.body[0], m_request_data.body.size());
    append_stream << content_stream.str();
    append_stream.close();
  }
}
