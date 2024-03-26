
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

bool Server::checkCmdArgs(std::deque<std::string> &parsedCmd) {

	std::string cmdType = parsedCmd[0];
	size_t argsNeed = 0;
	
	if (cmdType == "QUIT" || cmdType == "MODE" || cmdType == "PONG")
		argsNeed = 1;
	else if (cmdType == "PASS" || cmdType == "NICK" || cmdType == "PART" || cmdType == "JOIN" || cmdType == "PRIVMSG" || cmdType == "TOPIC" || cmdType == "PING" || cmdType == "PART")
		argsNeed = 2;
	else if (cmdType == "KICK" || cmdType == "INVITE")
		argsNeed = 3;
	else if (cmdType == "USER")
		argsNeed = 5;

	return (parsedCmd.size() == argsNeed);

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

		if (client.isRegistered()) {

			if (_cmdMap.find(cmdType) == _cmdMap.end())
				sendMessageToClient(client.getSocket(), ERR_UNKNOWNCOMMAND(client.getNickname(), parsedCmd[0]));
			else if (checkCmdArgs(parsedCmd))
				(this->*_cmdMap[cmdType])(parsedCmd, client);
			else
				sendMessageToClient(client.getSocket(), ERR_NEEDMOREPARAMS(client.getNickname(), cmdType));
		}
		else {

			if (cmdType == "PASS" || ((cmdType == "NICK" || cmdType == "USER") && client.isPassed()))
				(this->*_cmdMap[cmdType])(parsedCmd, client);
			else if (_cmdMap.find(cmdType) != _cmdMap.end())
				sendMessageToClient(client.getSocket(), ERR_NOTREGISTERED(client.getNickname()));

		}

		parsedCmd.clear();
	}

	client.clearBuffer();
}

void Server::sayHelloToClient(Client &client) {
	
	int clientSocket = client.getSocket();
	std::string nickname = client.getNickname();

	sendMessageToClient(clientSocket, RPL_WELCOME(nickname));
	sendMessageToClient(clientSocket, RPL_YOURHOST(nickname));
	sendMessageToClient(clientSocket, RPL_CREATED(nickname, "2024-03-31"));
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

	std::map<int, Client *>::iterator it = _clients.begin();

	for (; it != _clients.end(); it++) {
		if ((*it).second->getNickname() == parsedCmd[1] && (*it).second != &client) {
			sendMessageToClient(client.getSocket(), ERR_NICKNAMEINUSE(client.getUsername(), parsedCmd[1]));
			return ;
		}
	}

	if (parsedCmd[1].find(" ") != std::string::npos || parsedCmd[1].find("&") != std::string::npos ||
	parsedCmd[1].find("#") != std::string::npos || parsedCmd[1].find(",") != std::string::npos) {
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

void Server::PING(std::deque<std::string> &parsedCmd, Client &client) {
	sendMessageToClient(client.getSocket(), ":ircserv PONG " + parsedCmd[1] + "\n");
}

void Server::QUIT(std::deque<std::string> &parsedCmd, Client &client) { (void)parsedCmd, (void)client; }
void Server::JOIN(std::deque<std::string> &parsedCmd, Client &client) {
    // param : JOIN #channel [key]   (key is optional)
        // 인자가 2개까지만 처리하도록 하겠음.
	if (parsedCmd.size() < 2) {
		sendMessageToClient(client.getSocket(), ERR_NEEDMOREPARAMS(client.getNickname(), "JOIN"));
		return ;
	}
    // 0. check if channel name is valid : (ex: #channel)
    std::string channelName = parsedCmd[1];
	// ',' 가 있으면, 쉼표를 제외한 앞부분만 채널명으로 사용
	if (channelName.find(",") != std::string::npos)
		channelName = channelName.substr(0, channelName.find(","));
	// 채널명이 #으로 시작하지 않거나, 그 다음이 숫자나 영문자로 이루어지지 않으면 에러
	if (channelName[0] != '#') {
		sendMessageToClient(client.getSocket(), ERR_BADCHANMASK(channelName));
		return ;
	}
	for (size_t i = 1; i < channelName.size(); i++) {
		if (!std::isalnum(channelName[i])) {
			sendMessageToClient(client.getSocket(), ERR_BADCHANMASK(channelName));
			return ;
		}
	}
	// 1. check if client joined too many channels
	if (client.getChannels().size() >= MAX_CHANNELS) {
		sendMessageToClient(client.getSocket(), ERR_TOOMANYCHANNELS(client.getNickname(), channelName));
		return ;
	}
    // 2. check if channel not exist
	if (_channels.find(channelName) == _channels.end()) {
		_channels[channelName] = new Channel(channelName, client.getSocket(), client);
	} else {
		// 3. check if channel is invite only
		// 4. check if channel is moderated
		// 5. check if channel is +i or +t or +k or +o or +l
		int putUsersResult = _channels[channelName]->putUsers(client.getSocket(), client);
		if (putUsersResult == 0)
			return ;
		else if (putUsersResult == -1) {
			sendMessageToClient(client.getSocket(), ERR_CHANNELISFULL(client.getNickname(), channelName));
			return ;
		}
	}
	// 6. join channel
	client.getChannels()[channelName] = _channels[channelName];
	sendMessageToClient(client.getSocket(), ":" + client.getNickname() + "!" + client.getUsername() + "@localhost JOIN :" + channelName + "\r\n");
	// 7. send RPL_NAMREPLY
	sendMessageToClient(client.getSocket(), RPL_NAMREPLY(client.getNickname(), channelName, _channels[channelName]->getUsersList()));
	// 8. send RPL_ENDOFNAMES
	sendMessageToClient(client.getSocket(), RPL_ENDOFNAMES(client.getNickname(), channelName));
}
void Server::WHO(std::deque<std::string> &parsedCmd, Client &client) { (void)parsedCmd, (void)client; }
void Server::MODE(std::deque<std::string> &parsedCmd, Client &client) {
	// param : MODE #channel [mode] [nickname]
	if (parsedCmd.size() < 3) {
		sendMessageToClient(client.getSocket(), ERR_NEEDMOREPARAMS(client.getNickname(), "MODE"));
		return ;
	}
	std::string channelName = parsedCmd[1];
	std::string mode = parsedCmd[2];
	// 1. check if channelName is channel
	if (channelName[0] != '#') {
		sendMessageToClient(client.getSocket(), ERR_NOSUCHCHANNEL(client.getNickname(), channelName));
		return ;
	}
	// 2. check if channel exist
	if (_channels.find(channelName) == _channels.end()) {
		sendMessageToClient(client.getSocket(), ERR_NOSUCHCHANNEL(client.getNickname(), channelName));
		return ;
	}
	// 3. check if client is in channel
	if (_channels[channelName]->isClientIn(client.getSocket()) == 0) {
		sendMessageToClient(client.getSocket(), ERR_NOTONCHANNEL(client.getNickname(), channelName));
		return ;
	}
	// 4. check if client is operator
	if (_channels[channelName]->getOpers().find(client.getSocket()) == _channels[channelName]->getOpers().end()) {
		sendMessageToClient(client.getSocket(), ERR_CHANOPRIVSNEEDED(client.getNickname(), channelName));
		return ;
	}
	// 5. check if mode is valid
	if (mode.size() < 2) {
		sendMessageToClient(client.getSocket(), ERR_UNKNOWNMODE(client.getNickname(), mode));
		return ;
	}
	// 6. check if mode is valid
	if (mode[0] != '+' && mode[0] != '-') {
		sendMessageToClient(client.getSocket(), ERR_UNKNOWNMODE(client.getNickname(), mode));
		return ;
	}
	// 7. check if mode is valid
	if (mode[1] != 'o' && mode[1] != 'i' && mode[1] != 't' && mode[1] != 'l' && mode[1] != 'k') {
		sendMessageToClient(client.getSocket(), ERR_UMODEUNKNOWNFLAG(client.getNickname()));
		return ;
	}
	// 8. check if mode is valid
	if (mode[1] == 'o') {
		std::map<int, Client *>::iterator it = _clients.begin();
		for (; it != _clients.end(); it++) {
			if ((*it).second->getNickname() == parsedCmd[3])
				break ;
		}
		if (it == _clients.end()) {
			sendMessageToClient(client.getSocket(), ERR_NOSUCHNICK(client.getNickname(), parsedCmd[3]));
			return ;
		}
		Client &nickClient = *(*it).second;
		if (_channels[channelName]->getUsers().find(nickClient.getSocket()) == _channels[channelName]->getUsers().end()) {
			sendMessageToClient(client.getSocket(), ERR_USERNOTINCHANNEL(client.getNickname(), nickClient.getNickname(), channelName));
			return ;
		}
		if (mode[0] == '+') {
			_channels[channelName]->putOpers(nickClient.getSocket());
			sendMessageToClient(client.getSocket(), RPL_CHANNELMODEIS(client.getNickname(), channelName, mode));
		} else {
			_channels[channelName]->delOpers(nickClient.getSocket());
			sendMessageToClient(client.getSocket(), RPL_CHANNELMODEIS(client.getNickname(), channelName, mode));
		}
	} else if (mode[1] == 'i') {
		_channels[channelName]->changeI(mode[0]);
		sendMessageToClient(client.getSocket(), RPL_CHANNELMODEIS(client.getNickname(), channelName, mode));
	} else if (mode[1] == 't') {
		_channels[channelName]->changeT(mode[0]);
		sendMessageToClient(client.getSocket(), RPL_CHANNELMODEIS(client.getNickname(), channelName, mode));
	} else if (mode[1] == 'l') {
		if (mode[0] == '+') {
			_channels[channelName]->setMaxNumClients(std::stoi(parsedCmd[3]));
			sendMessageToClient(client.getSocket(), RPL_CHANNELMODEIS(client.getNickname(), channelName, mode));
		} else {
			_channels[channelName]->setMaxNumClients(0);
			sendMessageToClient(client.getSocket(), RPL_CHANNELMODEIS(client.getNickname(), channelName, mode));
		}
	} else if (mode[1] == 'k') {
		if (mode[0] == '+') {
			_channels[channelName]->setKey(parsedCmd[3]);
			sendMessageToClient(client.getSocket(), RPL_CHANNELMODEIS(client.getNickname(), channelName, mode));
		} else {
			_channels[channelName]->setKey("");
			sendMessageToClient(client.getSocket(), RPL_CHANNELMODEIS(client.getNickname(), channelName, mode));
		}
	}
}
void Server::INVITE(std::deque<std::string> &parsedCmd, Client &client) {
	// param : INVITE nickname #channel
	if (parsedCmd.size() < 3) {
		sendMessageToClient(client.getSocket(), ERR_NEEDMOREPARAMS(client.getNickname(), "INVITE"));
		return ;
	}
	// 확인해보니까 여기선 채널명 검증을 안하더라
	std::string channelName = parsedCmd[2];
	// ',' 가 있으면, 쉼표를 제외한 앞부분만 채널명으로 사용
	if (channelName.find(",") != std::string::npos)
		channelName = channelName.substr(0, channelName.find(","));
	// 1. check if channel exist
	if (_channels.find(channelName) == _channels.end()) {
		sendMessageToClient(client.getSocket(), ERR_NOSUCHCHANNEL(client.getNickname(), channelName));
		return ;
	}
	// 2. check if client is in channel
	if (_channels[channelName]->isClientIn(client.getSocket()) == 0) {
		sendMessageToClient(client.getSocket(), ERR_NOTONCHANNEL(client.getNickname(), channelName));
		return ;
	}
	// // 3. check if client is operator
	// if (_channels[channelName]->getOpers().find(client.getSocket()) == _channels[channelName]->getOpers().end()) {
	// 	sendMessageToClient(client.getSocket(), ERR_CHANOPRIVSNEEDED(client.getNickname(), channelName));
	// 	return ;
	// }
	std::string nickname = parsedCmd[1];
	// 4. check if nickname to be invited to channel exists in server
	std::map<int, Client *>::iterator it = _clients.begin();
	for (; it != _clients.end(); it++) {
		if ((*it).second->getNickname() == nickname)
			break ;
	}
	if (it == _clients.end()) {
		sendMessageToClient(client.getSocket(), ERR_NOSUCHNICK(client.getNickname(), nickname));
		return ;
	}
	Client &nickClient = *(*it).second;
	// 5. check if nickClient is already in channel and
	// put nickClient in _invite if nickClient is never invited
	int putInviteResult = _channels[channelName]->putInvite(nickClient.getSocket());
	if (putInviteResult == 0) {
		sendMessageToClient(client.getSocket(), ERR_USERONCHANNEL(client.getNickname(), nickname, channelName));
		return ;
	} else if (putInviteResult == 1)
		nickClient.getInvited().push_back(channelName);
	// 6. invite nickname to channel
	sendMessageToClient(client.getSocket(), RPL_INVITING(client.getNickname(), nickname, channelName));
	sendMessageToClient(client.getSocket(), ":" + client.getNickname() + "!" + client.getUsername() + "@localhost INVITE " + nickname + " :" + channelName + "\r\n");
}
void Server::KICK(std::deque<std::string> &parsedCmd, Client &client) { (void)parsedCmd, (void)client; }
void Server::TOPIC(std::deque<std::string> &parsedCmd, Client &client) { (void)parsedCmd, (void)client; }
void Server::PART(std::deque<std::string> &parsedCmd, Client &client) { (void)parsedCmd, (void)client; }
void Server::PRIVMSG(std::deque<std::string> &parsedCmd, Client &client) { (void)parsedCmd, (void)client; }

