
#include "Server.hpp"
#include "Client.hpp"
#include "Types.hpp"
#include "RPL.hpp"

Server::Server(char *port, char *password): _password(password) {
	printLog("setup");
	openServerSocket(port);
	makeKqueueReady();
	makeCmdMap();
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
					excuteCommands(_clients[curSocket]);
					_clients[curSocket].clearBuffer();
				}
			}
		}
	}
}

void Server::printLog(std::string logMsg) {
	std::cout << "[SERVER] " << logMsg << "\n";
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

void  Server::parseCommand(std::string str, std::deque<std::string> &parsedCmd) {

	std::string colonArg;

	if (str.find(":") != std::string::npos) {
		colonArg = str.substr(str.find(":") + 1, str.size());
		str = str.substr(0, str.find(":"));
	}

	std::stringstream ss(str);
	std::string tmp;

	while (ss >> tmp)
		parsedCmd.push_back(tmp);
	
	if (colonArg != "")
		parsedCmd.push_back(colonArg);
}

void Server::excuteCommands(Client& client)
{
	std::deque<std::string>		commands;
	std::deque<std::string>		parsedCmd;
	std::string					cmdType;

	parseByChar(client.getBuffer(), '\n', commands);

	for (size_t i = 0; i < commands.size(); i++) {

		parseCommand(commands[i], parsedCmd);

		for(size_t i = 0; i < parsedCmd.size(); i++)
			std::cout << parsedCmd[i] << std::endl;

		if (parsedCmd.size() == 0)
			continue ;

		cmdType = parsedCmd[0];

		if (_cmdMap.find(cmdType) == _cmdMap.end() && !client.isRegistered())
			sendError(421, client);
		else if (cmdType == "PASS" || client.isRegistered())
			(this->*_cmdMap[cmdType])(parsedCmd, client);
		else
			sendError(451, client);

		parsedCmd.clear();
	}

	client.clearBuffer();
}

void Server::sendError(int errNum, Client &client) { 
	(void) errNum, (void) client;
	std::cout << "error: invalid command" << std::endl;
}

void Server::NICK(std::deque<std::string> &parsedCmd, Client &client) {

	client.setNickname(parsedCmd[1]);
}

void Server::PASS(std::deque<std::string> &parsedCmd, Client &client) {

	(void)parsedCmd;
	sayHelloToClient(client.getSocket());
	client.setPassed();
}

void Server::USER(std::deque<std::string> &parsedCmd, Client &client) {

	client.setUsername(parsedCmd[1]);
}
