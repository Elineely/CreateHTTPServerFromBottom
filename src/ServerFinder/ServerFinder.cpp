#include "ServerFinder.hpp"
#include <map>
#include <iostream>

ServerFinder::ServerFinder() {}

ServerFinder::~ServerFinder() {}

t_server find_server_name(std::string server_part, std::string port_part, std::vector<t_server>& servers)
{
    std::vector<t_server> candidates;

    for (size_t i = 0; i < servers.size(); ++i) //이름 포트 일치하는애 찾아
    {
        std::vector<std::string> current_server_name = servers[i].server_name;
        if (current_server_name.size() == 0)
        {
            //서버 네임이 설정되어 있지 않은 경우, 후보자
            candidates.push_back(servers[i]);
            continue ;
        }
        if (current_server_name[0].size() == server_part.size())
        {
            if (current_server_name[0] == server_part)
            {
                for (size_t k = 0; k < servers[i].listen.size(); ++k)
                {
                    if (servers[i].listen[k] == port_part)
                        return (servers[i]);
                }
                throw NOT_FOUND_404; //이름이 매칭 된적이 있는데 해당 서버에서 매치되는 포트가 없는 경우 not_found
            }
        }
    }
    //host명이 아닌, IP로 접근했을 경우, 서버 네임이 설정되어 있지 않은 후보자 가운데서 포트 매칭을 확인
    for (size_t i = 0; i < candidates.size(); ++i)
    {
        for (size_t k = 0; k < servers[i].listen.size(); ++k)
        {
            if (candidates[i].listen[k] == port_part)
            {
                return (candidates[i]);
            }
        }
    }
    throw NOT_FOUND_404; //이외의 모든 경우 not_found_404
}

ServerFinder::ServerFinder(Request& request, std::vector<t_server>& servers)
{
    if (servers.size() == 0)
    {
      throw INTERNAL_SERVER_ERROR_500;
    }
    std::map<std::string, std::string>::iterator temp = request.headers.find("host");
    try
    {
        if (temp == request.headers.end()) 
        {
            //header에 host:가 안 들어온 경우, BAD REQUEST 취급
            throw BAD_REQUEST_400;
        }
        std::string host_origin = request.headers.find("host")->second;
        size_t pos_colone = host_origin.find(":");
        if (pos_colone == std::string::npos)
        {
            throw BAD_REQUEST_400;
        }
        std::string server_part = host_origin.substr(0, pos_colone);
        std::string port_part = host_origin.substr(pos_colone + 1);
        this->current_server = find_server_name(server_part, port_part, servers);
    }
    catch (StatusCode code)
    {
        request.status = code;
        return;
    }
}

t_server& ServerFinder::get_server(void)
{
return (current_server);
}
