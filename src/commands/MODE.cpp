
#include "Server.hpp"
#include "RPL.hpp"
#include "Utils.hpp"

void Server::MODE(std::deque<std::string> &parsedCmd, Client &client) {

	// param : MODE #channel [mode] [nickname]
	if (parsedCmd.size() < 2) {
		sendMessageToClient(client.getSocket(), ERR_NEEDMOREPARAMS(client.getNickname(), "MODE"));
		return ;
	}

	std::string channelName = parsedCmd[1];
	std::string opt;

	if (parsedCmd.size() > 3)
		opt = parsedCmd[3];

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

	// 6. check if mode is valid
	std::string mode = parsedCmd[2].substr(0, 2);
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

	switch (mode[1]) {

		case 'k':
		
			if (mode[0] == '+') {

				if (parsedCmd.size() < 4) {
					sendMessageToClient(client.getSocket(), ERR_NEEDMOREPARAMS(client.getNickname(), "MODE"));
					return ;
				}

				if (!util::isAlNum(opt)) {
					sendMessageToClient(client.getSocket(), ERR_INVALIDKEY(client.getNickname(), channelName));
					return ;
				}

				_channels[channelName]->setKey(opt);
			} 
			else {
				_channels[channelName]->setKey("");
			}

			break ;

		case 'l':

			if (mode[0] == '+') {
			
				if (parsedCmd.size() < 4) {
					sendMessageToClient(client.getSocket(), ERR_NEEDMOREPARAMS(client.getNickname(), "MODE"));
					return ;
				}

				if (!util::isPosNum(opt))
					return ;

				double limit;
				std::stringstream ss(opt); ss >> limit;

				if (limit > MAX_CLIENTS)
					return ;

				_channels[channelName]->setMaxNumClients(static_cast<int>(limit));
			}
			else {
				_channels[channelName]->setMaxNumClients(MAX_CLIENTS);
			}
			
			break ;


		case 'i':
			_channels[channelName]->changeI(mode[0]);
			break ;


		case 'o': {
			if (parsedCmd.size() < 4) {
				sendMessageToClient(client.getSocket(), ERR_NEEDMOREPARAMS(client.getNickname(), "MODE"));
				return ;
			}

			if (_clientsNick.find(parsedCmd[3]) == _clientsNick.end()) {
				sendMessageToClient(client.getSocket(), ERR_NOSUCHNICK(client.getNickname(), opt));
				return ;
			}

			Client &targetClient = *_clientsNick[parsedCmd[3]];

			if (_channels[channelName]->isClientIn(targetClient.getSocket()) == 0) {
				sendMessageToClient(client.getSocket(), ERR_USERNOTINCHANNEL(client.getNickname(), targetClient.getNickname(), channelName));
				return ;
			}

			if (mode[0] == '+')
				_channels[channelName]->putOpers(targetClient.getSocket());
			else
				_channels[channelName]->delOpers(targetClient.getSocket());

			break ;
		}


		case 't':
			_channels[channelName]->changeT(mode[0]);
			break ;

	}

	if (mode == "+k" || mode == "+l" || mode == "+o" || mode == "-o")
		sendMessageToChannel(*_channels[channelName], RPL_MODE(client.getClientInfo(), channelName, mode, opt));
	else
		sendMessageToChannel(*_channels[channelName], RPL_MODE(client.getClientInfo(), channelName, mode, ""));

}
