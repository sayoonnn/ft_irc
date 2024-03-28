
#pragma once

#include <sys/socket.h>
#include <sys/event.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <signal.h>

#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>
#include <sstream>
#include <iostream>
#include <vector>
#include <list>
#include <map>
#include <deque>

#include "Client.hpp"
#include "Channel.hpp"

#define EVENT_SIZE 64
#define BUFFER_SIZE 1024

class Server {

	private:
		std::map<int, Client *> _clients;
		std::map<std::string, Client *> _clientsNick;
		std::map<std::string, Channel *> _channels;

		typedef	void (Server::*commandFunc)(std::deque<std::string>&, Client &);
		std::map<std::string, commandFunc> _cmdMap;

		int 				_servPort;
		struct sockaddr_in	_servAddr;
		std::string 		_password;

		struct kevent 		_eventSetting;
		struct kevent		_eventList[EVENT_SIZE];

		std::deque<std::string>		_MOTD;

		Server();
		Server(const Server&);
		Server &operator=(const Server&);

		void closeServer(std::string errMsg);

		void makeCmdMap();
		void openServerSocket(char *);

		void makeKqueueReady();
		void addClientKq(int);
		void removeClientKq(int);

		void acceptClient();
		void removeClient(int);
		void removeClientUnexpect(int);

		void sayHelloToClient(Client &);
		int recvMessageFromClient(int);
		void sendMessageToClient(int, std::string);

		void printServerLog(std::string);
		void printClientLog(int, std::string);

		void excuteCommands(Client&);

		void sendMessageToChannel(Channel&, const std::string&, std::list<int>&);
		void sendMessageToChannel(Channel&, const std::string&);
		int	isNoClientInChannel(const std::string&);

		bool PASS(std::deque<std::string>&, Client &);
		void NICK(std::deque<std::string>&, Client &);
		void USER(std::deque<std::string>&, Client &);
		void PING(std::deque<std::string>&, Client &);
		void QUIT(std::deque<std::string>&, Client &);
		void JOIN(std::deque<std::string>&, Client &);
		void MODE(std::deque<std::string>&, Client &);
		void INVITE(std::deque<std::string>&, Client &);
		void KICK(std::deque<std::string>&, Client &);
		void TOPIC(std::deque<std::string>&, Client &);
		void PRIVMSG(std::deque<std::string>&, Client &);
		void PART(std::deque<std::string>&, Client &);

		void loadMOTD();
		void handleSignal();
		void setPassword(char *);

	public:
		static int 			_servSocket;
		static int			_kqueue;

		Server(char *, char *);
		~Server();

		void runServer();
		

};
