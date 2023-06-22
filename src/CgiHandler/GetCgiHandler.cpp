#include "CgiHandler.hpp"

// canonical form

GetCgiHandler::GetCgiHandler(Request& request_data, Response& response_data)
    : CgiHandler(request_data, response_data)
{
}

GetCgiHandler::~GetCgiHandler() {}

GetCgiHandler::GetCgiHandler(const GetCgiHandler& obj) : CgiHandler(obj)
{
  if (this != &obj)
  {
    m_env_list = obj.m_env_list;
    m_env_list_parameter = obj.m_env_list_parameter;
    m_to_child_fds[READ] = obj.m_to_child_fds[READ];
    m_to_child_fds[WRITE] = obj.m_to_child_fds[WRITE];
    m_to_parent_fds[READ] = obj.m_to_parent_fds[READ];
    m_to_parent_fds[WRITE] = obj.m_to_parent_fds[WRITE];
    m_pid = obj.m_pid;
  }
}

// member functions

void GetCgiHandler::executeCgi()
{
  close(m_to_child_fds[WRITE]);
  close(m_to_parent_fds[READ]);

  if (dup2(m_to_child_fds[READ], STDIN_FILENO) == -1)
  {
    Log::print(ERROR, "failed to dup2(%d, %d)", m_to_child_fds, STDIN_FILENO);
    close(m_to_child_fds[READ]);
    close(m_to_parent_fds[WRITE]);
    exit(EXIT_FAILURE);
  }

  if (dup2(m_to_parent_fds[WRITE], STDOUT_FILENO) == -1)
  {
    Log::print(ERROR, "failed to dup2(%d, %d)", m_to_parent_fds, STDOUT_FILENO);
    close(m_to_child_fds[READ]);
    close(m_to_parent_fds[WRITE]);
    exit(EXIT_FAILURE);
  }

   if (dup2(m_to_parent_fds[WRITE], STDERR_FILENO) == -1)
  {
    Log::print(ERROR, "failed to dup2(%d, %d)", m_to_parent_fds, STDOUT_FILENO);
    close(m_to_child_fds[READ]);
    close(m_to_parent_fds[WRITE]);
    exit(EXIT_FAILURE);
  }

  close(m_to_child_fds[READ]);
  close(m_to_parent_fds[WRITE]);

  setCgiEnv();

  const char* cgi_bin_path = m_response_data.cgi_bin_path.c_str();
  std::string cgi_file =
      m_response_data.root_path + "/" + m_response_data.file_name;
  const char* argv[] = {cgi_bin_path, cgi_file.c_str(), NULL};
  const char** envp = &m_env_list_parameter[0];

  if (execve(cgi_bin_path, const_cast<char* const*>(argv),
             const_cast<char* const*>(envp)) == RETURN_ERROR)
  {
    Log::print(ERROR, "Failed to execve function => strerrno: %s", strerror(errno));
    std::vector<char> error_message = makeErrorPage();
    write(STDOUT_FILENO, &error_message[0], error_message.size());
    exit(EXIT_FAILURE);
  }
}

void GetCgiHandler::outsourceCgiRequest(void)
{
  try
  {
    if (m_response_data.file_exist == false)
    {
      throw ExecutionException();
    }

    pipeAndFork();

    if (m_pid == CHILD_PROCESS)
    {
      executeCgi();
    }
    else
    {
      close(m_to_child_fds[READ]);
      close(m_to_child_fds[WRITE]);
      close(m_to_parent_fds[WRITE]);

      m_response_data.read_pipe_fd = m_to_parent_fds[READ];
      m_response_data.write_pipe_fd = m_to_child_fds[WRITE];
      m_response_data.cgi_child_pid = m_pid;
    }
  }
  catch (const std::exception& e)
  {
    Log::print(ERROR, "catch error %s", e.what());
    m_response_data.cgi_flag = false;
    m_response_data.status_code = NOT_IMPLEMENTED_501;
  }
}
