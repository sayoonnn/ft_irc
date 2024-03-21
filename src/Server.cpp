
#include "Server.hpp"
#include "Types.hpp"

Server::Server(char *port, char *password)
: _servSocket(-1), _password(password), _kqueue(-1)
{
	printServerLog("setup");
	openServerSocket(port);
	makeKqueueReady();
	makeCmdMap();
	loadMOTD();
}

Server::~Server() {

	std::map<int, Client *>::iterator it = _clients.begin();

	for (; it != _clients.end(); it++)
		delete (*it).second;

	printServerLog("closed");
}

void Server::runServer() {
	
	printServerLog("started");
	
	int eventCnt;
	
	eventCnt = kevent(_kqueue, NULL, 0, _eventList, EVENT_SIZE, NULL);
	if (eventCnt == FAIL)
		closeServer("cannot load event!");

	while (true) {
		eventCnt = kevent(_kqueue, NULL, 0, _eventList, EVENT_SIZE, NULL);

		for (int i = 0; i < eventCnt; i++) {

			struct kevent curEvent = _eventList[i];
			int curSocket = _eventList[i].ident;

			if (curSocket == _servSocket)
				acceptClient();
			else if (curEvent.filter == EVFILT_READ) {
				if (recvMessageFromClient(curSocket) == SUCCESS && _clients[curSocket]->isBufferEndNl()) {
					excuteCommands(*_clients[curSocket]);
					_clients[curSocket]->clearBuffer();
				}
			}
		}
	}
}

void Server::printServerLog(std::string logMsg) {
	std::cout << BLUE << std::right << std::setw(15) << "SERVER ! ";
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
	_cmdMap["PING"] = &Server::USER;
	_cmdMap["PONG"] = &Server::USER;
	_cmdMap["QUIT"] = &Server::USER;
	_cmdMap["JOIN"] = &Server::USER;
	_cmdMap["WHO"] = &Server::USER;
	_cmdMap["MODE"] = &Server::USER;
	_cmdMap["INVITE"] = &Server::USER;
	_cmdMap["KICK"] = &Server::USER;
	_cmdMap["TOPIC"] = &Server::USER;
	_cmdMap["PART"] = &Server::USER;
	_cmdMap["PRIVMSG"] = &Server::USER;
	_cmdMap["PART"] = &Server::USER;

}

void Server::closeServer(std::string errMsg) {

	std::cerr << "error: " << errMsg << std::endl;
	
	if (_servSocket != -1)
		close(_servSocket);
	
	if (_kqueue != -1)
		close(_kqueue);

	exit(EXIT_FAILURE);
}

void Server::loadMOTD() {

	std::ifstream tmp("./include/MOTD.txt");
	std::string buffer;

	if (!tmp.is_open())
		closeServer("cannot open MOTD");

	while (std::getline(tmp, buffer))
		_MOTD.push_back(buffer);

	tmp.close();
}
