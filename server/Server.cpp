#include "Server.hpp"

void add_event_to_change_list(std::vector<struct kevent>& change_list,
    uintptr_t       ident,  /* identifier for this event */
	int16_t         filter, /* filter for event */
	uint16_t        flags , /* general flags */
	uint32_t        fflags, /* filter-specific flags */
	intptr_t        data  ,/* filter-specific data */
	void            *udata)
{
    struct kevent temp_event;

    EV_SET(&temp_event, ident, filter, flags, fflags, data, udata);
    change_list.push_back(temp_event);
};


void sendClientHttpMessage(int clientNum)
{
    char protocol[] = "HTTP/1.1 200 OK\r\n";
    char server[] = "Server:Linux Web Server \r\n";
    char date[] = "Date: Wed, 03 May 2023 05:53:31 GM \r\n";
    char cnt_len[] ="Content-length:10\r\n";
    char cnt_type[BUF_SIZE];
    char buff[BUF_SIZE];

    FILE *send_file;

    // here is size of 
    sprintf(cnt_type, "Content-Type: text/plain\r\n\r\n<h1>hi<h1>");
    send(clientNum, protocol, strlen(protocol) , 0);
    send(clientNum, server, strlen(server), 0);
    send(clientNum, date, strlen(date) , 0);
    send(clientNum, cnt_len, strlen(cnt_len) , 0);
    send(clientNum, cnt_type, strlen(cnt_type), 0);
}

Server::Server()
{
    std::cout << "Server Constructor Call" << std::endl;
}

Server::Server(Config serverConf)
{
    std::cout << "Server Constructor Call" << std::endl;
    server_sock = socket(PF_INET, SOCK_STREAM, 0);
    if (server_sock == -1)
    {
        strerror(errno);
        return ;
    }
    std::cout << "success socket" << std::endl;

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port=htons(serverConf.getServerPort());
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    int kq;
    kq = kqueue();
    if (kq == -1)
    {
        std::cout << "kqueue() error :" << strerror(errno) << std::endl;
        return ;
    }
    if(bind(server_sock, (const struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1)
    {
        std::cout << "bind error" << std::endl;
        return ;
    }
    std::cout << "success bind" << std::endl;
    if(listen(server_sock, 5) == -1)
    {
        std::cout << "listen error" << std::endl;
        return ;
    }
    std::cout << "success listen" << std::endl;

    std::map<int, std::string> clients;
    std::vector<struct kevent> change_list;
    struct kevent event_list[8];

    add_event_to_change_list(change_list, server_sock, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);

    int current_events;
    struct kevent* current_event;
    int client_sock;
    while(1)
    {
        current_events = kevent(kq, &change_list[0], change_list.size(), event_list, MAX_KEVENT_LISTEN, NULL);
        if (current_events == -1)
        {
            std::cout << "kevent() error" << std::endl;
        }
        change_list.clear();
        for (int i=0; i <current_events; ++i)
        {
            current_event = &event_list[i];

            if (current_event->flags & EV_ERROR)
            {
                if (current_event->ident == server_sock)
                    std::cout << "server socket error" << std::endl;
                else 
                    std::cout << "client socket error" << std::endl;
                    // disconnect anything
                
            }
            // can read buff event
            else if (current_event->filter == EVFILT_READ)
            {
                // to server 
                if (current_event->ident == server_sock)
                {
                    client_sock = accept(server_sock, (struct sockaddr* )&client_addr, (socklen_t *)&client_addr_size);
                    if (client_sock == -1)
                    {
                        strerror(errno);
                        return ;
                    }
                    std::cout << "새로운 클라이언트가 연결 되었습니다." << std::endl;
                    fcntl(client_sock, F_SETFL, O_NONBLOCK);
                    add_event_to_change_list(change_list, client_sock, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
                    clients[client_sock] = "";
                }
                // to client
                else if (clients.find(current_event->ident) != clients.end())
                {
                    char buff[1024];
                    int recv_size = recv(current_event->ident, buff, sizeof(buff), 0);

                    if (recv_size <= 0)
                    {
                        if (recv_size < 0)
                            std::cout << "client read error" << std::endl;
                        // disconnect_ anything
                    }
                    std::cout << "client message to server : " << buff << std::endl;
                    sendClientHttpMessage(current_event->ident);
                }
            }
            // can write buff event
            // else if (EVFILT_WRITE)
            // {

            // }
        }
    }
    shutdown(server_sock, SHUT_RDWR);
    close(server_sock);
    
	return ;
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
