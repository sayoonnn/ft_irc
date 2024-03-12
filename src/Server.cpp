
#include "Server.hpp"
#include "Types.hpp"

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
		exit(EXIT_FAILURE);
	}

	bzero(&_eventSetting, sizeof(_eventSetting));
	EV_SET(&_eventSetting, _servSocket, EVFILT_READ, EV_ADD | EV_ENABLE , 0, 0, NULL);

	int ret = kevent(_kqueue, &_eventSetting, 1, NULL, 0, NULL);
	if (ret == FAIL) {
		std::cerr << "error: cannot register event on kqueue" << std::endl;
		// kque, socket닫기
		exit(1);
	}
}

void Server::addClientKq(int clientSocket) {

	bzero(&_eventSetting, sizeof(_eventSetting));
	EV_SET(&_eventSetting, clientSocket, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);

	int ret = kevent(_kqueue, &_eventSetting, 1, NULL, 0, NULL);
	if (ret == FAIL) {
		std::cerr << "error: cannot register event on kqueue" << std::endl;
		// kque, socket닫기
		exit(1);
	}
}

void Server::removeClientKq(int clientSocket) {

	bzero(&_eventSetting, sizeof(_eventSetting));
	EV_SET(&_eventSetting, clientSocket, EVFILT_READ, EV_DISABLE, 0, 0, NULL);

	int ret = kevent(_kqueue, &_eventSetting, 1, NULL, 0, NULL);
	if (ret == FAIL) {
		std::cerr << "error: cannot register event on kqueue" << std::endl;
		// kque, socket닫기
		exit(1);
	}
}

void Server::runServer() {
	
	printLog("started");
	
	int eventCnt;
	
	eventCnt = kevent(_kqueue, NULL, 0, _eventList, EVENT_SIZE, NULL);
	// if (eventCnt == FAIL) {
	// 	std::cerr << "error: cannot load events" << std::endl;
	// 	// kque, socket닫기
	// 	exit(1);
	// }

	while (true) {
		eventCnt = kevent(_kqueue, NULL, 0, _eventList, EVENT_SIZE, NULL);

		for (int i = 0; i < eventCnt; i++) {

			struct kevent curEvent = _eventList[i];
			int curFd = _eventList[i].ident;

			if (curEvent.filter == EVFILT_READ) {
				if (curFd == _servSocket) {
					int clientSocket;

					clientSocket = accept(_servSocket, NULL, NULL);
					std::cout << "client " << clientSocket << " connected" << "\n";

					fcntl(clientSocket, F_SETFL, O_NONBLOCK);
					addClientKq(clientSocket);
				}
				else {
					char buffer[1024];
					int n = recv(curFd, buffer, 1024, 0);
					if (n > 0) {
						std::cout << "client " << curFd << ": " << std::string(buffer, n - 1) << "\n";
						std::string tmp("CAP * LS :multi-prefix\n");
						send(curFd, tmp.c_str(), tmp.size(), 0);
						std::cout << "sent" << std::endl;
					}
					else if (errno == ECONNRESET || errno == EPIPE) {
						errno = 0;
						std::cout << "client" << curFd << " disconnected" << std::endl;
						removeClientKq(curFd);
						close(curFd);
					}
				}
			}
			else if (curEvent.flags & EV_EOF) {
				removeClientKq(curFd);
				close(curFd);
			}
		}
	}

}

void Server::printLog(std::string logMsg) {
	std::cout << "[SERVER] " << logMsg << "\n";
}
