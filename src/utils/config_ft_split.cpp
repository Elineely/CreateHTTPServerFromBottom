#include "../../include/utils.hpp"

std::vector<std::string> config_ft_split(std::string readLine,
                                             char delimiter) {
  std::string buffer;
  std::istringstream iss(readLine);
  std::vector<std::string> split_readLine;

  while (iss >> buffer) {
    split_readLine.push_back(buffer);     // 절삭된 문자열을 vector에 저장
  }

  return split_readLine;
}
