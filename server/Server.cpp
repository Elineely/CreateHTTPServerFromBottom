#include "Server.hpp"

Server::Server()
{
    std::cout << "Server Constructor Call" << std::endl;
}

Server::Server(Parser serverConf)
{
    std::cout << "Server Constructor Call" << std::endl;
}

Server::Server(const Server& other)
{
    std::cout << "Server Constructor Call" << std::endl;
    *this = other;
}

Server::~Server()
{
    std::cout << "Server Destructor Call" << std::endl;
}

Server& Server::operator=(const Server& other)
{
    if (this == &other)
        return *this;
    std::cout << "Server Assignment Operator Call" << std::endl;
    return *this;
}
