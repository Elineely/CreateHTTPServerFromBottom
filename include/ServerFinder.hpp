#ifndef SERVERFINDER_HPP
# define SERVERFINDER_HPP

# include <vector>
# include "Request.hpp"
# include "Config.hpp"

class ServerFinder
{
    private:
    ServerFinder();
    ServerFinder(const ServerFinder& origin);
    ServerFinder& operator=(ServerFinder const& origin);
    t_server current_server;

    public:
    ~ServerFinder();
    ServerFinder(Request& request, std::vector<t_server>& servers);
    t_server& get_server(void);

};

#endif