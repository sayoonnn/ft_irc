
#include "Server.hpp"
#include "RPL.hpp"
#include "Utils.hpp"

void Server::MODE(std::deque<std::string> &parsedCmd, Client &client) {

	// param : MODE #channel [mode] [nickname]
	if (parsedCmd.size() < 3) {
		sendMessageToClient(client.getSocket(), ERR_NEEDMOREPARAMS(client.getNickname(), "MODE"));
		return ;
	}

	std::string channelName = parsedCmd[1];
	std::string mode = parsedCmd[2].substr(0, 2);
	std::string opt;

	if (parsedCmd.size() > 3)
		opt = parsedCmd[3];

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
	if (_channels[channelName]->isClientIn(client.getSocket()) == 1) {
		sendMessageToClient(client.getSocket(), ERR_CHANOPRIVSNEEDED(client.getNickname(), channelName));
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

	switch (mode[1]) {

		case 'k':
		
			if (mode[0] == '+') {

				if (parsedCmd.size() < 4) {
					sendMessageToClient(client.getSocket(), ERR_NEEDMOREPARAMS(client.getNickname(), "MODE"));
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

				_channels[channelName]->setMaxNumClients(std::stoi(opt));
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

	if (mode == "+k" || mode == "+l")
		sendMessageToChannel(*_channels[channelName], RPL_CHANNELMODEIS(client.getNickname(), channelName, mode + " " + opt));
	else
		sendMessageToChannel(*_channels[channelName], RPL_CHANNELMODEIS(client.getNickname(), channelName, mode));

	// if (mode[1] == 'i') {
	// 	_channels[channelName]->changeI(mode[0]);
	// 	sendMessageToChannel(*_channels[channelName], RPL_CHANNELMODEIS(client.getNickname(), channelName, mode));
	// } 

	// else if (mode[1] == 't') {
	// 	_channels[channelName]->changeT(mode[0]);
	// 	sendMessageToChannel(*_channels[channelName], RPL_CHANNELMODEIS(client.getNickname(), channelName, mode));
	// } 

	// else if (mode[1] == 'o') {
	// 	if (parsedCmd.size() < 4) {
	// 		sendMessageToClient(client.getSocket(), ERR_NEEDMOREPARAMS(client.getNickname(), "MODE"));
	// 		return ;
	// 	}
	// 	std::map<int, Client *>::iterator it = _clients.begin();
	// 	for (; it != _clients.end(); it++) {
	// 		if ((*it).second->getNickname() == parsedCmd[3])
	// 			break ;
	// 	}
	// 	if (it == _clients.end()) {
	// 		sendMessageToClient(client.getSocket(), ERR_NOSUCHNICK(client.getNickname(), parsedCmd[3]));
	// 		return ;
	// 	}
	// 	Client &nickClient = *(*it).second;
	// 	if (_channels[channelName]->getUsers().find(nickClient.getSocket()) == _channels[channelName]->getUsers().end()) {
	// 		sendMessageToClient(client.getSocket(), ERR_USERNOTINCHANNEL(client.getNickname(), nickClient.getNickname(), channelName));
	// 		return ;
	// 	}
	// 	if (mode[0] == '+') {
	// 		_channels[channelName]->putOpers(nickClient.getSocket());
	// 		sendMessageToClient(client.getSocket(), RPL_CHANNELMODEIS(client.getNickname(), channelName, mode));
	// 	} else {
	// 		_channels[channelName]->delOpers(nickClient.getSocket());
	// 		sendMessageToClient(client.getSocket(), RPL_CHANNELMODEIS(client.getNickname(), channelName, mode));
	// 	}
	// }

	// else if (mode[1] == 'l') {
	// 	if (mode[0] == '+') {
			
	// 		if (parsedCmd.size() < 4) {
	// 			sendMessageToClient(client.getSocket(), ERR_NEEDMOREPARAMS(client.getNickname(), "MODE +l"));
	// 			return ;
	// 		}

	// 		if ()

	// 		_channels[channelName]->setMaxNumClients(std::stoi(parsedCmd[3]));
	// 		sendMessageToClient(client.getSocket(), RPL_CHANNELMODEIS(client.getNickname(), channelName, mode));
	// 	} else {
	// 		_channels[channelName]->setMaxNumClients(MAX_CLIENTS);
	// 		sendMessageToClient(client.getSocket(), RPL_CHANNELMODEIS(client.getNickname(), channelName, mode));
	// 	}
	// }

	// else if (mode[1] == 'k') {
	// 	if (mode[0] == '+') {
	// 		if (parsedCmd.size() < 4) {
	// 			sendMessageToClient(client.getSocket(), ERR_NEEDMOREPARAMS(client.getNickname(), "MODE"));
	// 			return ;
	// 		}
	// 		_channels[channelName]->setKey(parsedCmd[3]);
	// 		sendMessageToClient(client.getSocket(), RPL_CHANNELMODEIS(client.getNickname(), channelName, mode));
	// 	} 
	// 	else {
	// 		_channels[channelName]->setKey("");
	// 		sendMessageToClient(client.getSocket(), RPL_CHANNELMODEIS(client.getNickname(), channelName, mode));
	// 	}
	// }
}