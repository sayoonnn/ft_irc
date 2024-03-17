
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

void Server::openServerSocket(char *port) {

	std::stringstream ss(port);
	double tmp;

	ss >> tmp;
	if (ss.fail()) {
		std::cerr << "error: " << port << ": invalid port number" << std::endl;
		exit(EXIT_FAILURE);
	}

	_servPort = static_cast<int>(tmp);

	_servSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (_servSocket == FAIL) {
		std::cerr << "error: cannot open socket" << std::endl;
		exit(EXIT_FAILURE);
	}

	_servAddr.sin_family = AF_INET;
	_servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	_servAddr.sin_port = htons(_servPort);

	if (bind(_servSocket, (struct sockaddr *)&_servAddr, sizeof(_servAddr)) == FAIL) {
		std::cerr << "error: cannot bind socket" << std::endl;
		close(_servSocket);
		exit(EXIT_FAILURE);
	}

	if (listen(_servSocket, SOMAXCONN) == FAIL) {
		std::cerr << "error: socket cannot listen" << std::endl;
		close(_servSocket);
		exit(EXIT_FAILURE);
	}

}

void Server::makeKqueueReady() {

	_kqueue = kqueue();
	if (_kqueue == FAIL) {
		std::cerr << "error: cannot make kqueue" << std::endl;
		// 서버 종료 함수
		std::exit(EXIT_FAILURE);
	}

	std::memset(&_eventSetting, 0, sizeof(_eventSetting));
	EV_SET(&_eventSetting, _servSocket, EVFILT_READ, EV_ADD | EV_ENABLE , 0, 0, NULL);

	int ret = kevent(_kqueue, &_eventSetting, 1, NULL, 0, NULL);
	if (ret == FAIL) {
		std::cerr << "error: cannot register event on kqueue" << std::endl;
		// kque, socket닫기
		std::exit(1);
	}
}

void Server::addClientKq(int clientSocket) {

	std::memset(&_eventSetting, 0, sizeof(_eventSetting));
	EV_SET(&_eventSetting, clientSocket, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);

	int ret = kevent(_kqueue, &_eventSetting, 1, NULL, 0, NULL);
	if (ret == FAIL) {
		std::cerr << "error: cannot register event on kqueue" << std::endl;
		// kque, socket닫기
		std::exit(1);
	}
}

void Server::removeClientKq(int clientSocket) {

	std::memset(&_eventSetting, 0, sizeof(_eventSetting));
	EV_SET(&_eventSetting, clientSocket, EVFILT_READ, EV_DISABLE, 0, 0, NULL);

	int ret = kevent(_kqueue, &_eventSetting, 1, NULL, 0, NULL);
	if (ret == FAIL) {
		std::cerr << "error: cannot register event on kqueue" << std::endl;
		// kque, socket닫기
		std::exit(1);
	}
}

void Server::sayHelloToClient(int clientFd) {
	sendMessageToClient(clientFd, RPL_WELCOME(std::string("sayoon")));
	sendMessageToClient(clientFd, RPL_YOURHOST(std::string("sayoon")));
	sendMessageToClient(clientFd, RPL_CREATED(std::string("sayoon"), std::string("2020-11-11")));
	sendMessageToClient(clientFd, RPL_MYINFO(std::string("sayoon")));
}

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

void Server::recvNAddToBuffer(int clientFd) {

	int length;
	char buffer[BUFFER_SIZE];
	std::string tmp;

	length = recv(clientFd, buffer, BUFFER_SIZE - 1, 0);

	if (length == 0)
		return (false);

	buffer[length] = 0;
	tmp += buffer;

	while (length == BUFFER_SIZE - 1) {
		buffer[length] = 0;
		tmp += buffer;
		length = recv(clientFd, buffer, BUFFER_SIZE - 1, 0);	
	}

	_clients[clientFd].addToBuffer(tmp);
}

void Server::sendMessageToClient(int clientFd, std::string message) {
	
	ssize_t n;

	n = send(clientFd, message.c_str(), message.size(), 0);
	if (n < 0) {
		std::cerr << "error: cannot send" << std::endl;
		exit(1);
	}
}

void Server::printLog(std::string logMsg) {
	std::cout << "[SERVER] " << logMsg << "\n";
}

void Server::cmdNick() {
	
}
