
#pragma once

#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sstream>
#include <iostream>
#include <vector>

class Server {

	private:
		int 				_servSocket;
		int 				_servPort;
		struct sockaddr_in	_servAddr;
		std::string 		_password;

		std::vector<int> fds;

		Server();
		Server(const Server&);
		Server &operator=(const Server&);

		void makeServerReady(char *);

	public:
		Server(char *, char *);
		~Server();

		void runServer();

};
