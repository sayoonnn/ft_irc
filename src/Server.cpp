
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
			int curFd = _eventList[i].ident;

			if (curFd == _servSocket) {	
				int clientSocket;

				clientSocket = accept(_servSocket, NULL, NULL);
				std::cout << "[Client " << clientSocket << "]: connected" << "\n";

				fcntl(clientSocket, F_SETFL, O_NONBLOCK);
				addClientKq(clientSocket);
				_clients[clientSocket] = Client(clientSocket);
			}
			else if (curEvent.filter == EVFILT_READ) {
				recvNAddToBuffer(curFd);

				if (_clients[curFd].isBufferEndNl()) {
					
				}
			}
			else if (curEvent.flags & EV_EOF) {
				std::cout << "[Client " << curFd << "]: disconnected" << "\n";
				removeClientKq(curFd);
				_clients.erase(_clients.find(curFd));
				close(curFd);
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

