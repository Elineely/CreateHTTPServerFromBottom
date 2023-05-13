#ifndef SERVER
# define SERVER

// thread header
# include <pthread.h>
# include <sys/time.h>

// socket header
#include <iostream>
#include <sys/time.h>
#include <sys/event.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

// std::container header
#include <list>
#include <vector>

// common I/O header
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <sstream>
#include <stdio.h>

// Paser header
#include "Config.hpp"
#include "Parser.hpp"

#define BUF_SIZE 1024
#define MAX_KEVENT_LISTEN 8

class Server  {
	private:
		int server_sock;
		int client_addr_size;
		struct sockaddr_in serv_addr;
		struct sockaddr_in client_addr;
		Config serverConf;
		Server();

	public:
		Server(Config serverConf);
		Server(const Server& a);
		~Server();
		Server& operator=(const Server& a);

};

#endif 