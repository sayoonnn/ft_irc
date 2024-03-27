
#include "Server.hpp"
#include "Types.hpp"
#include "RPL.hpp"

void Server::openServerSocket(char *port) {

	std::stringstream ss(port);
	double tmp;

	ss >> tmp;
	if (ss.fail())
		closeServer("invalid port number");

	_servPort = static_cast<int>(tmp);

	_servSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (_servSocket == FAIL)
		closeServer("cannot open socket");

	_servAddr.sin_family = AF_INET;
	_servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	_servAddr.sin_port = htons(_servPort);

	if (bind(_servSocket, (struct sockaddr *)&_servAddr, sizeof(_servAddr)) == FAIL)
		closeServer("socket cannot bind");

	if (listen(_servSocket, SOMAXCONN) == FAIL)
		closeServer("socket cannot listen");
}

void Server::acceptClient() {

	int clientSocket;

	clientSocket = accept(_servSocket, NULL, NULL);
	printClientLog(clientSocket, "connected");

	fcntl(clientSocket, F_SETFL, O_NONBLOCK);
	addClientKq(clientSocket);

	Client *tmp = new Client(clientSocket);
	tmp->setIpAddr();
	_clients[clientSocket] = tmp;
}

void Server::removeClient(int clientSocket) {

	std::stringstream ss;

	ss << clientSocket;
	printServerLog("Client " + ss.str() + " disconnected");

	Client *client = _clients[clientSocket];

	std::map<std::string, Client *>::iterator	iterClientsNick = _clientsNick.find(client->getNickname());
	if (iterClientsNick != _clientsNick.end())
		_clientsNick.erase(iterClientsNick);

	std::map<int, Client *>::iterator			iterClients = _clients.find(client->getSocket());
	if (iterClients != _clients.end())
		_clients.erase(iterClients);

	removeClientKq(clientSocket);
	close(clientSocket);
	delete iterClients->second;
}

void Server::removeClientUnexpect(int clientSocket) {
	
	std::deque<std::string> tmp;

	tmp.push_back("QUIT");
	tmp.push_back("EOF from client");

	QUIT(tmp, *_clients[clientSocket]);
	removeClient(clientSocket);
}

int Server::recvMessageFromClient(int clientSocket) {

	int length;
	char buffer[BUFFER_SIZE];
	std::string tmp;

	length = recv(clientSocket, buffer, BUFFER_SIZE - 1, 0);

	if (length <= 0) {
		removeClientUnexpect(clientSocket);
		return (FAIL);
	}

	buffer[length] = 0;
	tmp += buffer;

	while (length == BUFFER_SIZE - 1) {
		buffer[length] = 0;
		tmp += buffer;
		length = recv(clientSocket, buffer, BUFFER_SIZE - 1, 0);	
	}

	_clients[clientSocket]->addToBuffer(tmp);

	return (SUCCESS);
}

void Server::sendMessageToClient(int clientSocket, std::string message) {
	
	ssize_t n;

	printServerLog(message);
	n = send(clientSocket, message.c_str(), message.size(), 0);
	if (n <= 0)
		removeClientUnexpect(clientSocket);
}

