
#include "Server.hpp"
#include "RPL.hpp"

void Server::MODE(std::deque<std::string> &parsedCmd, Client &client) {

	// param : MODE #channel [mode] [nickname]
	if (parsedCmd.size() < 2) {
		sendMessageToClient(client.getSocket(), ERR_NEEDMOREPARAMS(client.getNickname(), "MODE"));
		return ;
	}

	std::string channelName = parsedCmd[1];
	// 1. check if channelName is channel
	if (channelName[0] != '#') {
		if (_clientsNick.find(channelName) == _clientsNick.end()) {
			sendMessageToClient(client.getSocket(), ERR_NOSUCHNICK(client.getNickname(), channelName));
			return ;
		}
		if (client.getNickname() != channelName) {
			sendMessageToClient(client.getSocket(), ERR_USERSDONTMATCH(client.getNickname()));
			return ;
		}
		if (parsedCmd.size() != 2) {
			sendMessageToClient(client.getSocket(), ERR_UMODEUNKNOWNFLAG(client.getNickname()));
			return ;
		}
		sendMessageToClient(client.getSocket(), RPL_UMODEIS(client.getNickname(), ""));
		return ;
	}

	// 2. check if channel exist
	if (_channels.find(channelName) == _channels.end()) {
		sendMessageToClient(client.getSocket(), ERR_NOSUCHCHANNEL(client.getNickname(), channelName));
		return ;
	}

	if (parsedCmd.size() == 2) {
		std::string			info = "";
		std::string			flag = "+";

		if (_channels[channelName]->getT() == true)
			flag += "t";
		if (_channels[channelName]->getI() == true)
			flag += "i";
		if (_channels[channelName]->getMaxNumClients() != MAX_CLIENTS)
		{
			std::stringstream	infoStream;
			flag += "l";
			infoStream << _channels[channelName]->getMaxNumClients();
			info += " " + infoStream.str();
		}
		if (_channels[channelName]->getKey() != "")
		{
			flag += "k";
			info += " " + _channels[channelName]->getKey();
		}
		if (_channels[channelName]->isClientIn(client.getSocket()) != 0)
			flag += info;
		sendMessageToClient(client.getSocket(), RPL_CHANNELMODEIS(client.getNickname(), channelName, flag));
		return ;
	}

	std::string mode = parsedCmd[2];

	// 3. check if client is in channel
	if (_channels[channelName]->isClientIn(client.getSocket()) == 0) {
		sendMessageToClient(client.getSocket(), ERR_NOTONCHANNEL(client.getNickname(), channelName));
		return ;
	}

	// 4. check if client is operator
	if (_channels[channelName]->isClientIn(client.getSocket()) == 1) {
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
	// i, t : no parameter
	// o : 1 parameter
	// l, k : 0 or 1 parameter
	if (mode[1] == 'i') {
		_channels[channelName]->changeI(mode[0]);
		sendMessageToClient(client.getSocket(), RPL_CHANNELMODEIS(client.getNickname(), channelName, mode));
	} 
	else if (mode[1] == 't') {
		_channels[channelName]->changeT(mode[0]);
		sendMessageToClient(client.getSocket(), RPL_CHANNELMODEIS(client.getNickname(), channelName, mode));
	} 
	else if (mode[1] == 'o') {
		if (parsedCmd.size() < 4) {
			sendMessageToClient(client.getSocket(), ERR_NEEDMOREPARAMS(client.getNickname(), "MODE"));
			return ;
		}
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
	}
	else if (mode[1] == 'l') {
		if (mode[0] == '+') {
			if (parsedCmd.size() < 4) {
				sendMessageToClient(client.getSocket(), ERR_NEEDMOREPARAMS(client.getNickname(), "MODE"));
				return ;
			}
			_channels[channelName]->setMaxNumClients(std::stoi(parsedCmd[3]));
			sendMessageToClient(client.getSocket(), RPL_CHANNELMODEIS(client.getNickname(), channelName, mode));
		} else {
			_channels[channelName]->setMaxNumClients(MAX_CLIENTS);
			sendMessageToClient(client.getSocket(), RPL_CHANNELMODEIS(client.getNickname(), channelName, mode));
		}
	}
	else if (mode[1] == 'k') {
		if (mode[0] == '+') {
			if (parsedCmd.size() < 4) {
				sendMessageToClient(client.getSocket(), ERR_NEEDMOREPARAMS(client.getNickname(), "MODE"));
				return ;
			}
			_channels[channelName]->setKey(parsedCmd[3]);
			sendMessageToClient(client.getSocket(), RPL_CHANNELMODEIS(client.getNickname(), channelName, mode));
		} else {
			_channels[channelName]->setKey("");
			sendMessageToClient(client.getSocket(), RPL_CHANNELMODEIS(client.getNickname(), channelName, mode));
		}
	}
}
