#include "Server.hpp"

#define SMALL_BUFF 100
#define BUF_SIZE 1024
struct sockaddr_in serv_addr;
std::list<int> g_listClient;


void sendClientHttpMessage(int clientNum)
{
    char protocol[] = "HTTP/1.1 200 OK\r\n";
    char server[] = "Server:Linux Web Server \r\n";
    char date[] = "Date: Wed, 03 May 2023 05:53:31 GM \r\n";
    char cnt_len[] ="Content-length:10\r\n";
    char cnt_type[SMALL_BUFF];
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

void serverMain() {
// int g_client_socks[CLNT_MAX];
	// int g_clnt_count = 0;
    int server_sock;
    int client_addr_size;
    struct sockaddr_in client_addr;

    server_sock = socket(PF_INET, SOCK_STREAM, 0);
    if (server_sock == -1)
    {
        strerror(errno);
        return ;
    }
    std::cout << "success socket" << std::endl;

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port=htons(10005);
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);

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

    int client_socket_fd;
    while(1)
    {
        client_socket_fd = accept(server_sock, (struct sockaddr* )&client_addr, (socklen_t *)&client_addr_size);
        if (client_socket_fd == -1)
        {
            strerror(errno);
            return ;
        }
        std::cout << "새로운 클라이언트가 연결 되었습니다." << std::endl;

        // critical section
        
        int recv_len;
        char buff[BUF_SIZE];
         while((recv_len = recv(client_socket_fd, buff, sizeof(buff), 0)))
        {
            
            // 1:N send all client on connected
            sendClientHttpMessage(client_socket_fd);
            std::cout << " ["<< client_socket_fd << "]  : " << buff << std::endl;
            memset(buff, 0, sizeof(buff));   
        }
        // pthread_t monitor_client;
        // if (pthread_create(&monitor_client, NULL, clientSockethread, &client_socket_fd))
		// 	return ;
        // client_communication_len++;
    }
    shutdown(server_sock, SHUT_RDWR);
    close(server_sock);
    
	return ;
}

int main()
{
    serverMain();
}
