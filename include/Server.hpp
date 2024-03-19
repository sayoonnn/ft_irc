
#pragma once

#include <sys/socket.h>
#include <sys/event.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

#include <iostream>
#include <string>
#include <sstream>
#include <iostream>
#include <vector>
#include <map>
#include <deque>

#include "Client.hpp"

#define EVENT_SIZE 64
#define BUFFER_SIZE 1024

class Server {

	private:
		typedef	void (Server::*commandFunc)(std::deque<std::string>&, Client &);

		std::map<int, Client> _clients;
		std::map<std::string, commandFunc> _cmdMap;


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

		int recvMessageFromClient(int);
		void sendMessageToClient(int, std::string);

		void printLog(std::string logMsg);

		void makeCmdMap();

		void excuteCommands(Client&);

		void parseCommand(std::string, std::deque<std::string>&);
		void parseByChar(std::string, char, std::deque<std::string>&);

		void PASS(std::deque<std::string>&, Client &);
		void NICK(std::deque<std::string>&, Client &);
		void USER(std::deque<std::string>&, Client &);

		
	public:
		Server(char *, char *);
		~Server();

		void runServer();
		

};
