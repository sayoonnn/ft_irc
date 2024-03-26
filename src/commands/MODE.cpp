
#include "Server.hpp"
#include "RPL.hpp"

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