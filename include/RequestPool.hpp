#ifndef REQUEST_POOL_HPP
#define REQUEST_POOL_HPP

#include <string>
#include <iostream>
#include <vector>

struct RequestPool
{
  std::vector<char> total_line;
  size_t line_len;
  size_t offset; // 마지막으로 찾은 \r\n 이후 + 2 지점

  // Default Constructor
  RequestPool(void) : line_len(0), offset(0)
  {
  }
 
  // Destructor
  ~RequestPool(void)
  {
  }

  // Copy constructor
  RequestPool(const RequestPool& src)
  {
    if (this != &src)
    {
      *this = src;
    }
  }

  // Copy assignment
  RequestPool& operator=(const RequestPool& rhs)
  {
    if (this != &rhs)
    {
      line_len = rhs.line_len;
      offset = rhs.offset;
      if (total_line.size() > 0)
      {
        total_line.clear();
      }
      if (rhs.total_line.size() > 0)
      {
        total_line.insert(total_line.begin(), rhs.total_line.begin(),
                          rhs.total_line.end());
      }
    }
    return (*this);
  }
};

#endif
