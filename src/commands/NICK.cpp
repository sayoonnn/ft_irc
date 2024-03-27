
#include "Server.hpp"
#include "RPL.hpp"

void Server::NICK(std::deque<std::string> &parsedCmd, Client &client) {

	if (parsedCmd.size() < 2) {
		sendMessageToClient(client.getSocket(), ERR_NEEDMOREPARAMS(client.getNickname(), "NICK"));
		return ;
	}
	
	std::string newNick = parsedCmd[1];
	std::string oldNick = client.getNickname();

	if (_clientsNick.find(newNick) != _clientsNick.end()) {
		sendMessageToClient(client.getSocket(), ERR_NICKNAMEINUSE(client.getUsername(), newNick));
		return ;
	}

	if (newNick.find(" ") != std::string::npos || newNick.find("&") != std::string::npos ||
	newNick.find("#") != std::string::npos || newNick.find(",") != std::string::npos) {
		sendMessageToClient(client.getSocket(), ERR_ERRONEUSNICKNAME(oldNick, newNick));
		return ;
	}

	std::map<std::string, Client *>::iterator nickIt = _clientsNick.find(oldNick);

	if (nickIt == _clientsNick.end())
		_clientsNick[newNick] = &client;
	else {
		_clientsNick.erase(nickIt);
		_clientsNick[newNick] = &client;
		sendMessageToClient(client.getSocket(), RPL_NICK(newNick, oldNick));
	}

	std::map<std::string, Channel *> joinedChannel = client.getChannels();
	std::map<std::string, Channel *>::iterator chanIt = joinedChannel.begin();

	std::list<int> fds;
	fds.push_back(client.getSocket());

	for (; chanIt != joinedChannel.end(); chanIt++)
		sendMessageToChannel(*(chanIt->second), RPL_NICK(newNick, oldNick), fds);

	client.setNickname(newNick);

	if (client.isPassed() && client.getUsername() != "" && !client.isRegistered()) {
		client.setRegistered();
		sayHelloToClient(client);
	}
}
