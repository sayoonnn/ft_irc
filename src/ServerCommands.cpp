
#include "Server.hpp"
#include "RPL.hpp"

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

void Server::parseByChar(std::string target, char delimeter, std::deque<std::string> &commands) {

	std::stringstream ss(target);
	std::string tmp;

	while (std::getline(ss, tmp, delimeter))
		commands.push_back(tmp);
}

void Server::excuteCommands(Client& client)
{
	std::deque<std::string>		commands;
	std::deque<std::string>		parsedCmd;
	std::string					cmdType;

	parseByChar(client.getBuffer(), '\n', commands);

	for (size_t i = 0; i < commands.size(); i++) {

		printClientLog(client.getSocket(), commands[i]);
		parseCommand(commands[i], parsedCmd);

		if (parsedCmd.size() == 0)
			continue ;

		cmdType = parsedCmd[0];

		if (cmdType == "PASS" || client.isRegistered())
			(this->*_cmdMap[cmdType])(parsedCmd, client);
		else if ((cmdType == "NICK" || cmdType == "USER") && client.isPassed())
			(this->*_cmdMap[cmdType])(parsedCmd, client);
		else if (_cmdMap.find(cmdType) == _cmdMap.end() && client.isRegistered())
			sendMessageToClient(client.getSocket(), ERR_UNKNOWNCOMMAND(client.getNickname(), parsedCmd[0]));
		else if (_cmdMap.find(cmdType) != _cmdMap.end() && !client.isRegistered())
			sendMessageToClient(client.getSocket(), ERR_NOTREGISTERED(client.getNickname()));

		parsedCmd.clear();
	}

	client.clearBuffer();
}

void Server::sayHelloToClient(Client &client) {
	
	int clientSocket = client.getSocket();
	std::string nickname = client.getNickname();

	sendMessageToClient(clientSocket, RPL_WELCOME(nickname));
	sendMessageToClient(clientSocket, RPL_YOURHOST(nickname));
	sendMessageToClient(clientSocket, RPL_CREATED(nickname, std::string("2024-03-31")));
	sendMessageToClient(clientSocket, RPL_MYINFO(nickname));
	
	sendMessageToClient(clientSocket, RPL_MOTDSTART(nickname));
	for (size_t i = 0; i < _MOTD.size(); i++)
		sendMessageToClient(clientSocket, RPL_MOTD(nickname, _MOTD[i]));
	sendMessageToClient(clientSocket, RPL_ENDOFMOTD(nickname));
}


void Server::NICK(std::deque<std::string> &parsedCmd, Client &client) {

	if (parsedCmd.size() < 2) {
		sendMessageToClient(client.getSocket(), ERR_NEEDMOREPARAMS(client.getNickname(), "NICK"));
		return ;
	}

	std::map<int, Client>::iterator it = _clients.begin();

	for(; it != _clients.end(); it++) {
		if ((*it).second.getNickname() == parsedCmd[1] && &(*it).second != &client) {
			sendMessageToClient(client.getSocket(), ERR_NICKNAMEINUSE(client.getUsername(), parsedCmd[1]));
			return ;
		}
	}

	if (parsedCmd[1].find(" ") != std::string::npos ||
	parsedCmd[1].find("&") != std::string::npos ||
	parsedCmd[1].find("#") != std::string::npos) {
		sendMessageToClient(client.getSocket(), ERR_ERRONEUSNICKNAME(client.getNickname(), parsedCmd[1]));
		return ;
	}

	client.setNickname(parsedCmd[1]);

	if (client.isPassed() && client.getUsername() != "" && !client.isRegistered()) {
		client.setRegistered();
		sayHelloToClient(client);
	}
}

void Server::PASS(std::deque<std::string> &parsedCmd, Client &client) {

	if (parsedCmd.size() < 2) {
		sendMessageToClient(client.getSocket(), ERR_NEEDMOREPARAMS(client.getNickname(), "PASS"));
		return ;
	}

	if (client.isPassed()) {
		sendMessageToClient(client.getSocket(), ERR_ALREADYREGISTERED(client.getNickname()));
		return ;
	}

	if (_password != parsedCmd[1]) {
		sendMessageToClient(client.getSocket(), ERR_PASSWDMISMATCH(client.getNickname()));
		return ;
	}

	client.setPassed();
}

void Server::USER(std::deque<std::string> &parsedCmd, Client &client) {

	if (parsedCmd.size() < 5) {
		sendMessageToClient(client.getSocket(), ERR_NEEDMOREPARAMS(client.getNickname(), "USER"));
		return ;
	}

	if (client.isRegistered()) {
		sendMessageToClient(client.getSocket(), ERR_ALREADYREGISTERED(client.getNickname()));
		return ;
	}

	client.setUsername(parsedCmd[1]);
	client.setRealname(parsedCmd[4]);

	if (client.isPassed() && client.getNickname() != "" && !client.isRegistered()) {
		client.setRegistered();
		sayHelloToClient(client);
	}
}


