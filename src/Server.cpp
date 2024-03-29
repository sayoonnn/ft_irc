
#include "Server.hpp"
#include "Types.hpp"
#include "Utils.hpp"

int Server::_servSocket = -1;
int Server::_kqueue = -1;

void signalHandler(int sig) {

	if (sig == SIGINT) {
		std::cout << BLUE << std::right << std::setw(13) << "SERVER ! ";
		std::cout << "closing Server..." << "\n" << RESET;
		close(Server::_servSocket);
		close(Server::_kqueue);
		exit(0);
	}

}

Server::Server(char *port, char *password) {

	setPassword(password);
	openServerSocket(port);
	makeKqueueReady();
	makeCmdMap();
	loadMOTD();
	handleSignal();
	printServerLog("running....");
}

Server::~Server() {

	{
		std::map<int, Client *>::iterator it = _clients.begin();

		for (; it != _clients.end(); it++)
			delete (*it).second;
	}

	{
		std::map<std::string, Channel *>::iterator it = _channels.begin();

		for (; it != _channels.end(); it++)
			delete (*it).second;
	}

	printServerLog("closed");
}

void Server::runServer() {
	
	// printServerLog("started");
	
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
				if (recvMessageFromClient(curSocket) == SUCCESS && _clients[curSocket]->isBufferEndNl())
					excuteCommands(*_clients[curSocket]);
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

	_cmdMap["NICK"] = &Server::NICK;
	_cmdMap["USER"] = &Server::USER;
	_cmdMap["PING"] = &Server::PING;
	_cmdMap["QUIT"] = &Server::QUIT;
	_cmdMap["JOIN"] = &Server::JOIN;
	_cmdMap["MODE"] = &Server::MODE;
	_cmdMap["INVITE"] = &Server::INVITE;
	_cmdMap["KICK"] = &Server::KICK;
	_cmdMap["TOPIC"] = &Server::TOPIC;
	_cmdMap["PART"] = &Server::PART;
	_cmdMap["PRIVMSG"] = &Server::PRIVMSG;
	_cmdMap["PART"] = &Server::PART;

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

void Server::handleSignal() {

	struct sigaction sa;

	sa.sa_handler = &signalHandler;
	sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;

    sigaction(SIGINT, &sa, NULL);
}

void Server::setPassword(char *password) {

	if (!util::isAlNum(std::string(password)))
		closeServer("invalid password");

	_password = std::string(password);

}
