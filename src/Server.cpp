
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
					_commands.excuteCommands(_clients[curSocket]);
					_clients[curSocket].clearBuffer();
				}
			}
		}
	}
}

void Server::printLog(std::string logMsg) {
	std::cout << "[SERVER] " << logMsg << "\n";
}

