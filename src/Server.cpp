
#include "Server.hpp"
#include "Client.hpp"
#include "Types.hpp"
#include "RPL.hpp"

Server::Server(char *port, char *password): _password(password) {
	printLog("setup");
	openServerSocket(port);
	makeKqueueReady();
}

Server::~Server() {
	printLog("closed");
}

Server::Server(__attribute__((unused)) const Server& copy) {}

Server &Server::operator=(__attribute__((unused)) const Server& copy) { return (*this); }

void Server::runServer() {
	
	printLog("started");
	
	int eventCnt;
	
	eventCnt = kevent(_kqueue, NULL, 0, _eventList, EVENT_SIZE, NULL);
	if (eventCnt == FAIL) {
		std::cerr << "error: cannot load events" << std::endl;
		// kque, socket닫기
		exit(1);
	}

	while (true) {
		eventCnt = kevent(_kqueue, NULL, 0, _eventList, EVENT_SIZE, NULL);

		for (int i = 0; i < eventCnt; i++) {

			struct kevent curEvent = _eventList[i];
			int curSocket = _eventList[i].ident;

			if (curSocket == _servSocket) {	
				int clientSocket;

				clientSocket = accept(_servSocket, NULL, NULL);
				std::cout << "[Client " << clientSocket << "]: connected" << "\n";

				fcntl(clientSocket, F_SETFL, O_NONBLOCK);
				addClientKq(clientSocket);
				_clients[clientSocket] = Client(clientSocket);
			}
			else if (curEvent.filter == EVFILT_READ) {
				if (recvNAddToBuffer(curSocket) == SUCCESS && 
				_clients[curSocket].isBufferEndNl()) {
					std::cout << _clients[curSocket].getBuffer();
					_clients[curSocket].clearBuffer();
				}
			}
		}
	}
}

void Server::checkExecCmd(std::string cmd) {
	std::vector<std::string> *tmp = parseCommand(cmd);

	(void)tmp;
}

std::vector<std::string> *Server::parseCommand(std::string cmd) {

	std::vector<std::string> *ret = new std::vector<std::string>;
	size_t prev;
	size_t delimeter;

	prev = 0;
	delimeter = cmd.find(" ");

	while (delimeter != std::string::npos) {
		ret->push_back(cmd.substr(prev, delimeter));
		prev = delimeter;
		delimeter = cmd.find(" ", prev);
	}

	return (ret);
}

void Server::printLog(std::string logMsg) {
	std::cout << "[SERVER] " << logMsg << "\n";
}

