#include <unistd.h>
#include <sys/wait.h>
#include <string>
#include <vector>
#include <iostream>

#include "../../include/CgiHandler.hpp"

int main(int argc, char** argv, char** env)
{

  PostCgiHandler post_handler;
  post_handler.setCgiEnv();
  post_handler.outsourceCgiRequest();
  
  for(int i = 0; i < post_handler.m_content_vector.size(); ++i)
  {
   std::cout << post_handler.m_content_vector[i];
  }
  std::cout << std::endl;

	return (0);
}
