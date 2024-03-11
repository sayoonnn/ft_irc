
#include "Server.hpp"
#include "Types.hpp"

Server::Server(char *port, char *password): _password(password) {
	std::cout << "[SERVER] setup" << "\n";
	makeServerReady(port);
	std::cout << "[SERVER] ready" << std::endl;
}

Server::~Server() {
	std::cout << "[SERVER] closed" << std::endl; 
}

Server::Server(__attribute__((unused)) const Server& copy) {}

Server &Server::operator=(__attribute__((unused)) const Server& copy) { return (*this); }

void Server::makeServerReady(char *port) {

	std::stringstream ss(port);
	double tmp;

	ss >> tmp;
	if (ss.fail()) {
		std::cerr << "error: " << port << ": invalid port number" << std::endl;
		exit(EXIT_FAILURE);
	}

	_servPort = static_cast<int>(tmp);

	if ((_servSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == FAIL) {
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

void Server::runServer() {
	std::cout << "[SERVER] started" << "\n";

	while (true) {
		int clientSocket; 
	}

}
