
#include "Server.hpp"
#include "Types.hpp"

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

void Server::recvNAddToBuffer(int clientFd) {

	int length;
	char buffer[BUFFER_SIZE];
	std::string tmp;

	length = recv(clientFd, buffer, BUFFER_SIZE - 1, 0);

	if (length == 0)
		return ;

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

void Server::sayHelloToClient(int clientFd) {
	sendMessageToClient(clientFd, RPL_WELCOME(std::string("sayoon")));
	sendMessageToClient(clientFd, RPL_YOURHOST(std::string("sayoon")));
	sendMessageToClient(clientFd, RPL_CREATED(std::string("sayoon"), std::string("2020-11-11")));
	sendMessageToClient(clientFd, RPL_MYINFO(std::string("sayoon")));
}
