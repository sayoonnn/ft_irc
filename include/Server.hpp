
#pragma once

#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <iostream>

class Server {

	private:
		int servSocket;
		int servPort;
		struct sockaddr_in servAddr;

	public:
		Server();
		~Server();
		Server(const Server& copy);
		Server& operator=(const Server& copy);

		

};
