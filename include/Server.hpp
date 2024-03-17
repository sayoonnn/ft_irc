
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
#include <map>
#include <fcntl.h>
#include <errno.h>

#include "Client.hpp"

#define EVENT_SIZE 64
#define BUFFER_SIZE 1024

class Server {

	private:

		std::map<int, Client> _clients;

		int 				_servSocket;
		int 				_servPort;
		struct sockaddr_in	_servAddr;
		std::string 		_password;

		int					_kqueue;
		struct kevent 		_eventSetting;
		struct kevent		_eventList[EVENT_SIZE];

		std::string			_serverBuffer;

		Server();
		Server(const Server&);
		Server &operator=(const Server&);

		void openServerSocket(char *);

		void makeKqueueReady();
		void addClientKq(int);
		void removeClientKq(int);

		void sayHelloToClient(int);

		void recvNAddToBuffer(int);
		void sendMessageToClient(int, std::string);

		void printLog(std::string logMsg);

		std::vector<std::string> *parseCommand(std::string);

		void checkExecCmd(std::string);

		void cmdNICK(Client &);
		void cmdPASS(Client &);
		void cmdUSER(Client &);
		
	public:
		Server(char *, char *);
		~Server();

		void runServer();
		

};
