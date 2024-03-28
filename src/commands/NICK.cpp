
#include "Server.hpp"
#include "RPL.hpp"

static bool checkNickname(std::string nick) {
	
	std::string notAllow(" &#:,");

	for (size_t i = 0; i < nick.size(); i++) {
		if (notAllow.find(nick[i]) != std::string::npos)
			return (false);
	}

	return (true);
}

void Server::NICK(std::deque<std::string> &parsedCmd, Client &client) {

	std::string username("*");

	if (client.getUsername() != "")
		username = client.getUsername();

	if (parsedCmd.size() < 2) {
		sendMessageToClient(client.getSocket(), ERR_NONICKNAMEGIVEN(username));
		return ;
	}
	
	std::string newNick = parsedCmd[1];
	std::string oldNick = client.getNickname();

	if (_clientsNick.find(newNick) != _clientsNick.end()) {
		sendMessageToClient(client.getSocket(), ERR_NICKNAMEINUSE(username, newNick));
		return ;
	}

	if (!checkNickname(newNick)) {
		sendMessageToClient(client.getSocket(), ERR_ERRONEUSNICKNAME(oldNick, newNick));
		return ;
	}

	std::map<std::string, Client *>::iterator nickIt = _clientsNick.find(oldNick);

	if (nickIt == _clientsNick.end())
		_clientsNick[newNick] = &client;
	else {
		_clientsNick.erase(nickIt);
		_clientsNick[newNick] = &client;
		sendMessageToClient(client.getSocket(), RPL_NICK(client.getClientInfo(), oldNick));
	}

	std::map<std::string, Channel *> joinedChannel = client.getChannels();
	std::map<std::string, Channel *>::iterator chanIt = joinedChannel.begin();

	std::list<int> fds;
	fds.push_back(client.getSocket());

	for (; chanIt != joinedChannel.end(); chanIt++)
		sendMessageToChannel(*(chanIt->second), RPL_NICK(client.getClientInfo(), oldNick), fds);

	client.setNickname(newNick);

	if (client.isPassed() && client.getUsername() != "" && !client.isRegistered()) {
		client.setRegistered();
		sayHelloToClient(client);
	}
}
