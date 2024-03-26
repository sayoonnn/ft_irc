
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

		if (client.isRegistered()) {

			if (_cmdMap.find(cmdType) == _cmdMap.end())
				sendMessageToClient(client.getSocket(), ERR_UNKNOWNCOMMAND(client.getNickname(), parsedCmd[0]));
			else
				(this->*_cmdMap[cmdType])(parsedCmd, client);
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


std::string	Server::splitComma(const std::string str)
{
	size_t	start = 0;
	size_t	i = 0;

	for (; i < str.size() && str[i] != '\n' && str[i] != '\r'; ++ i)
	{
		if (str[i] == ',')
		{
			if (start < i)
				return (str.substr(start, i - start));
		}
	}
	return (str.substr(start, i - start));
}

void	Server::sendMessageToChannel(Channel& channel, const std::string& message, std::list<int>& mask)
{
	std::map<int, Client*>	users = channel.getUsers();
	std::map<int, Client*>	opers = channel.getOpers();

	std::list<int>::iterator	iter = mask.begin();
	for (std::map<int, Client*>::iterator u_iter = users.begin(); u_iter != users.end(); ++ u_iter)
	{
		while (iter != mask.end() && *iter < u_iter->first)
			++ iter;
		if (iter == mask.end() || *iter > u_iter->first)
		{
			sendMessageToClient(u_iter->first, message);
			mask.insert(iter, u_iter->first);
		}
	}
	iter = mask.begin();
	for (std::map<int, Client*>::iterator o_iter = opers.begin(); o_iter != opers.end(); ++ o_iter)
	{
		while (iter != mask.end() && *iter < o_iter->first)
			++ iter;
		if (iter == mask.end() || *iter > o_iter->first)
		{
			sendMessageToClient(o_iter->first, message);
			mask.insert(iter, o_iter->first);
		}
	}
}

void	Server::sendMessageToChannel(Channel& channel, const std::string& message)
{
	std::map<int, Client*>	users = channel.getUsers();
	std::map<int, Client*> opers = channel.getOpers();

	for (std::map<int, Client*>::iterator u_iter = users.begin(); u_iter != users.end(); ++ u_iter)
		sendMessageToClient(u_iter->first, message);
	for (std::map<int, Client*>::iterator o_iter = opers.begin(); o_iter != opers.end(); ++ o_iter)
		sendMessageToClient(o_iter->first, message);
}

int	Server::isNoClientInChannel(const std::string& name)
{
	std::map<std::string, Channel *>::iterator chaIter;

	chaIter = _channels.find(name);
	if (chaIter == _channels.end())
	{
		std::cerr << "This is error in the inNoClientInChannel" << std::endl;
		return (-1);
	}
	Channel*	cha = chaIter->second;
	if (cha->numClients() != 0)
		return (0);
	delete cha;
	_channels.erase(chaIter);
	return (1);
}

