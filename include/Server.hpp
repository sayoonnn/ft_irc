
#pragma once

#include <sys/socket.h>
#include <sys/event.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sstream>
#include <iostream>
#include <vector>
#include <fcntl.h>
#include <errno.h>

#define EVENT_SIZE 64

class Server {

	private:
		int 				_servSocket;
		int 				_servPort;
		struct sockaddr_in	_servAddr;
		std::string 		_password;

		int					_kqueue;
		struct kevent 		_eventSetting;
		struct kevent		_eventList[EVENT_SIZE];

		std::vector<int> fds;

		Server();
		Server(const Server&);
		Server &operator=(const Server&);

		void openServerSocket(char *);
		void makeKqueueReady();

		void addClientKq(int);
		void removeClientKq(int);

		void printLog(std::string logMsg);

	public:
		Server(char *, char *);
		~Server();

		void runServer();
		

};
