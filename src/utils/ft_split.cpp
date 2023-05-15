#include "../../include/utils.hpp"

std::vector<std::string> ft_split(const std::string &str, char delimiter,
                                  int count) {
    std::string buf;
    std::stringstream ss(str);
    std::vector<std::string> vec;

    for (int curr = 0; curr < count; curr += 1) {
        if (std::getline(ss, buf, delimiter)) {
            vec.push_back(buf);
        }
    }

    if (std::getline(ss, buf, '\n')) {
        vec.push_back(buf);
    }

    return (vec);
}
