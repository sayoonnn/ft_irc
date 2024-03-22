
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
    std::string channel_name = parsedCmd[1];
	// ',' 가 있으면, 쉼표를 제외한 앞부분만 채널명으로 사용
	if (channel_name.find(",") != std::string::npos)
		channel_name = channel_name.substr(0, channel_name.find(","));
	// 채널명이 #으로 시작하지 않거나, 그 다음이 숫자나 영문자로 이루어지지 않으면 에러
	if (channel_name[0] != '#') {
		sendMessageToClient(client.getSocket(), ERR_BADCHANMASK(channel_name));
		return ;
	}
	for (size_t i = 1; i < channel_name.size(); i++) {
		if (!std::isalnum(channel_name[i])) {
			sendMessageToClient(client.getSocket(), ERR_BADCHANMASK(channel_name));
			return ;
		}
	}
	// 1. check if client joined too many channels
	if (client.getChannels().size() >= MAX_CHANNELS) {
		sendMessageToClient(client.getSocket(), ERR_TOOMANYCHANNELS(client.getNickname(), channel_name));
		return ;
	}
    // 2. check if channel not exist
	if (_channels.find(channel_name) == _channels.end()) {
		_channels[channel_name] = new Channel(channel_name, client.getSocket(), client);
	} else {
		// 3. check if channel is invite only
		// 4. check if channel is moderated
		// 5. check if channel is +i or +t or +k or +o or +l
		if (_channels[channel_name]->putUsers(client.getSocket(), client) == 0)
			return ;
		if (_channels[channel_name]->putUsers(client.getSocket(), client) == -1) {
			sendMessageToClient(client.getSocket(), ERR_CHANNELISFULL(client.getNickname(), channel_name));
			return ;
		}
	}
	// 6. join channel
	sendMessageToClient(client.getSocket(), ":" + client.getNickname() + "!~" + client.getUsername() + "@localhost JOIN :" + channel_name + "\n");
	// 7. send RPL_NAMREPLY
	sendMessageToClient(client.getSocket(), RPL_NAMREPLY(client.getNickname(), channel_name, _channels[channel_name]->getUsersList()));
	// 8. send RPL_ENDOFNAMES
	sendMessageToClient(client.getSocket(), RPL_ENDOFNAMES(client.getNickname(), channel_name));
}
void Server::WHO(std::deque<std::string> &parsedCmd, Client &client) { (void)parsedCmd, (void)client; }
void Server::MODE(std::deque<std::string> &parsedCmd, Client &client) { (void)parsedCmd, (void)client; }
void Server::INVITE(std::deque<std::string> &parsedCmd, Client &client) { (void)parsedCmd, (void)client; }
void Server::KICK(std::deque<std::string> &parsedCmd, Client &client) { (void)parsedCmd, (void)client; }
void Server::TOPIC(std::deque<std::string> &parsedCmd, Client &client) { (void)parsedCmd, (void)client; }
void Server::PART(std::deque<std::string> &parsedCmd, Client &client) { (void)parsedCmd, (void)client; }
void Server::PRIVMSG(std::deque<std::string> &parsedCmd, Client &client) { (void)parsedCmd, (void)client; }

