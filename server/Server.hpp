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

// common I/O header
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <sstream>
#include <stdio.h>

// Paser header
#include "Parser.hpp"

class Server  {
	private:
		Parser serverConf;
		Server();

	public:
		Server(Parser serverConf);
		Server(const Server& a);
		~Server();
		Server& operator=(const Server& a);

};

#endif 