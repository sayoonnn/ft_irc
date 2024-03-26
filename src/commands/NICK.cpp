
#include "Server.hpp"
#include "RPL.hpp"

void Server::NICK(std::deque<std::string> &parsedCmd, Client &client) {

	if (parsedCmd.size() < 2) {
		sendMessageToClient(client.getSocket(), ERR_NEEDMOREPARAMS(client.getNickname(), "NICK"));
		return ;
	}

	std::map<int, Client *>::iterator socketIt = _clients.begin();

	for (; socketIt != _clients.end(); socketIt++) {
		if ((*socketIt).second->getNickname() == parsedCmd[1] && (*socketIt).second != &client) {
			sendMessageToClient(client.getSocket(), ERR_NICKNAMEINUSE(client.getUsername(), parsedCmd[1]));
			return ;
		}
	}

	if (parsedCmd[1].find(" ") != std::string::npos || parsedCmd[1].find("&") != std::string::npos ||
	parsedCmd[1].find("#") != std::string::npos || parsedCmd[1].find(",") != std::string::npos) {
		sendMessageToClient(client.getSocket(), ERR_ERRONEUSNICKNAME(client.getNickname(), parsedCmd[1]));
		return ;
	}

	std::map<std::string, Client *>::iterator nickIt = _clientsNick.find(client.getNickname());

	if (nickIt == _clientsNick.end())
		_clientsNick[parsedCmd[1]] = &client;
	else {
		_clientsNick.erase(nickIt);
		_clientsNick[parsedCmd[1]] = &client;
		std::string tmp = ":" + client.getNickname() + " NICK " + parsedCmd[1] + "\n"; 
		sendMessageToClient(client.getSocket(), tmp);
	}

	client.setNickname(parsedCmd[1]);

	if (client.isPassed() && client.getUsername() != "" && !client.isRegistered()) {
		client.setRegistered();
		sayHelloToClient(client);
	}
}

