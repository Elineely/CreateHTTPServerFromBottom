#include "MethodHandler.hpp"

#include <dirent.h>
#include <fcntl.h>
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

// member fuctions for generating auto_index page
std::string MethodHandler::generateDate(const std::time_t& timestamp)
{
  char buffer[25];
  std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S",
                std::localtime(&timestamp));
  return buffer;
}
std::string MethodHandler::generateSize(const ssize_t& fileSize)
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
  if (dir == NULL)
  {
    throw INTERNAL_SERVER_ERROR_500;
  }
  entry = readdir(dir);
  while (entry != NULL)
  {
    std::string file_name = entry->d_name;
    struct stat file_stat;
    std::string file_path = target_directory + file_name;
    if (stat(file_path.c_str(), &file_stat) != -1)
    {
      FileInfo fileData;
      // stat구조체의 st_mode를 이용하여 디렉토리인지 파일인지 판별 후
      // 디렉토리라면 "/"를 뒤에 붙여주기 위함.
      if (S_ISDIR(file_stat.st_mode))
      {
        fileData.is_dir = "/";
      }
      else
      {
        fileData.is_dir = "";
      }
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
  int infile_fd;

  if (m_response_data.path_exist == false)
  {
    throw NOT_FOUND_404;
  }
  if (m_response_data.file_exist == true)
  {
    std::string infile_name;

    infile_name = m_response_data.file_path + m_response_data.file_name;
    infile_fd = open(infile_name.c_str(), O_RDONLY, 0644);
    if (infile_fd == -1)
    {
      throw NOT_FOUND_404;
    }
    fcntl(infile_fd, F_SETFL, O_NONBLOCK);
    m_response_data.static_read_file_fd = infile_fd;
  }
  else
  {
    if (m_response_data.file_name != "")
    {
      throw NOT_FOUND_404;
    }
    else if (m_response_data.index_exist == true)
    {
      std::string infile_name;

      infile_name = m_response_data.file_path + m_response_data.index_name;
      infile_fd = open(infile_name.c_str(), O_RDONLY, 0644);
      if (infile_fd == -1)
      {
        throw NOT_FOUND_404;
      }
      fcntl(infile_fd, F_SETFL, O_NONBLOCK);
      m_response_data.static_read_file_fd = infile_fd;
    }
    else if (m_response_data.auto_index == true)
    {
      autoIndexToBody(m_response_data.file_path);
    }
    else
    {
      throw NOT_FOUND_404;
    }
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
    if (m_response_data.index_name != "")
      m_response_data.file_name = m_response_data.index_name;
    else
      m_response_data.file_name = "default.temp";
    if (access(m_response_data.file_name.c_str(), F_OK) == 0)
      m_response_data.file_exist = true;
  }
  std::string target_file(m_response_data.file_path +
                          m_response_data.file_name);

  int outfile_fd;

  outfile_fd = open(target_file.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
  if (outfile_fd == -1)
  {
    throw INTERNAL_SERVER_ERROR_500;
  }
    fcntl(outfile_fd, F_SETFL, O_NONBLOCK);
  m_response_data.static_write_file_fd = outfile_fd;
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
  {
    if (std::remove(&target_file[0]) != 0)
    {
      throw INTERNAL_SERVER_ERROR_500;
    }
  }
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
    if (m_response_data.index_name != "")
      m_response_data.file_name = m_response_data.index_name;
    else
      m_response_data.file_name = "default.temp";
  }
  std::string target_file(m_response_data.file_path +
                          m_response_data.file_name);
  int outfile_fd;

  outfile_fd = open(target_file.c_str(), O_RDWR | O_CREAT | O_APPEND, 0644);
  if (outfile_fd == -1)
  {
    throw INTERNAL_SERVER_ERROR_500;
  }
  fcntl(outfile_fd, F_SETFL, O_NONBLOCK);
  lseek(outfile_fd, 0, SEEK_END);
  m_response_data.static_write_file_fd = outfile_fd;
}
