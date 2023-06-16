#include "ServerFinder.hpp"
#include <map>
#include <iostream>

ServerFinder::ServerFinder() {}

ServerFinder::~ServerFinder() {}

ServerFinder::ServerFinder(const ServerFinder& origin){};

ServerFinder& ServerFinder::operator=(ServerFinder const& origin) { return (*this); };

t_server find_server_name(std::string server_part, std::string port_part, std::vector<t_server>& servers)
{
    bool if_found = false;
    std::vector<t_server> candidates;

    for (int i = 0; i < servers.size(); ++i) //이름 포트 일치하는애 찾아
    {
        servers[i] = servers[i];
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
                for (int k = 0; k < servers[i].listen.size(); ++k)
                {
                    if (servers[i].listen[k] == port_part)
                        return (servers[i]);
                }
            }
        }
    }
    // if (server_part == "127.0.0.1") //host명이 아닌, IP로 접근했을 경우, 서버 네임이 설정되어 있지 않은 후보자 가운데서 포트 매칭을 확인
    // {
    for (int i = 0; i < candidates.size(); ++i)
    {
        for (int k = 0; k < servers[i].listen.size(); ++k)
        {
            if (servers[i].listen[k] == port_part)
            {
                return (candidates[i]);
            }
        }
    }
    // }
    throw BAD_REQUEST_400; //이외의 모든 경우 bad_request
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
            //header에 host:가 안 들어온 경우, server_name이 설정되어 있지 않은 블록에서 port 일치 여부 확인,
            //port 일치시 연결한다. -> 이 단에서는 host 헤더 없이는 port 번호를 알 수 가 없다. 
            //연결하려면 socket 쪽에서 port 번호를 받아야 한다. 
            //현재는 BadRequest 취급?? -> 이러면 servername을 설정하지 않은 모든 경우에 대해서 정상 동작을 할 수 없을 것 같다. 
            throw BAD_REQUEST_400;
            // request.status = BAD_REQUEST_400;
            // return ;
        }
        std::string host_origin = request.headers.find("host")->second;
        size_t pos_colone = host_origin.find(":");
        if (pos_colone == std::string::npos)
        {
            throw BAD_REQUEST_400;
        }
        std::string server_part = host_origin.substr(0, pos_colone);
        std::string port_part = host_origin.substr(pos_colone + 1);
        std::cout << "server : " << server_part << " , port_part : " << port_part << std::endl;
        // std::string host_origin = request.headers.find("host")->second;
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