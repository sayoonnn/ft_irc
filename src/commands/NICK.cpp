
#include "Server.hpp"
#include "RPL.hpp"

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

