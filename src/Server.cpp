
#include "Server.hpp"
#include "Client.hpp"
#include "Types.hpp"

Server::Server(char *port, char *password): _password(password) {
	printServerLog("setup");
	openServerSocket(port);
	makeKqueueReady();
	makeCmdMap();
	saveMOTD();
}

Server::~Server() {
	printServerLog("closed");
}

Server::Server(__attribute__((unused)) const Server& copy) {}

Server &Server::operator=(__attribute__((unused)) const Server& copy) { return (*this); }

void Server::runServer() {
	
	printServerLog("started");
	
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
				printClientLog(curSocket, "connected");

				fcntl(clientSocket, F_SETFL, O_NONBLOCK);
				addClientKq(clientSocket);
				_clients[clientSocket] = Client(clientSocket);
			}
			else if (curEvent.filter == EVFILT_READ) {
				if (recvMessageFromClient(curSocket) == SUCCESS && 
				_clients[curSocket].isBufferEndNl()) {
					excuteCommands(_clients[curSocket]);
					_clients[curSocket].clearBuffer();
				}
			}
		}
	}
}

void Server::printServerLog(std::string logMsg) {
	std::cout << BLUE << std::right << std::setw(15) << "SERVER | ";
	std::cout << logMsg << "\n" << RESET;
}

void Server::printClientLog(int clientNum, std::string logMsg) {

	std::string client;
	std::stringstream ss;

	ss << clientNum;

	client = "Client " + ss.str() + " | ";

	std::cout << std::right << std::setw(15) << client;
	std::cout << logMsg << "\n";
}


void Server::makeCmdMap() {
	_cmdMap["PASS"] = &Server::PASS;
	_cmdMap["NICK"] = &Server::NICK;
	_cmdMap["USER"] = &Server::USER;
}

void Server::parseByChar(std::string target, char delimeter, std::deque<std::string> &commands) {

	std::stringstream ss(target);
	std::string tmp;

	while (std::getline(ss, tmp, delimeter))
		commands.push_back(tmp);
}

void Server::saveMOTD() {

	std::ifstream tmp("./include/MOTD.txt");
	std::string buffer;

	if (!tmp.is_open()) {
		std::cerr << "error: cannot open MOTD file" << std::endl;
		exit(1);
	}

	while (std::getline(tmp, buffer))
		_MOTD += buffer + "\n";

}
