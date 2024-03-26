
#include "Server.hpp"
#include "RPL.hpp"

#define _QUIT(nickname, username, reason)			(":" + (nickname) + "!~" + (username) + "@localhost QUIT :" + (reason) + "\n")
#define CLOSING_LINK(nickname, username, reason)		(": ERROR :Closing Link: " + (nickname) + "[~" + (username) + "@localhost] (" + (reason) + ")\n")

void Server::QUIT(std::deque<std::string> &parsedCmd, Client &client) {

	std::string	nickname = "Unknown";
	std::string	username = "Unknown";
	std::string	reason = "";
	if (client.getNickname() != "")
		nickname = client.getNickname();
	if (client.getUsername() != "")
		username = client.getUsername();
	if (parsedCmd.size() > 1) {
		reason = parsedCmd.at(1);
	}

	std::map<std::string, Channel *>			channels = client.getChannels();
	std::map<std::string, Channel *>::iterator	iterChannel = channels.begin();
	Channel										*cha;
	std::list<int>								fds;

	for (; iterChannel != channels.end(); iterChannel ++)
	{
		cha = iterChannel->second;
		cha->delUsers(client.getSocket());
		if (isNoClientInChannel(iterChannel->first) == 0)
			sendMessageToChannel(*cha, _QUIT(nickname, username, reason), fds);
	}
	sendMessageToClient(client.getSocket(), CLOSING_LINK(nickname, username, reason));

	std::map<std::string, Client *>::iterator	iterClientsNick = _clientsNick.find(client.getNickname());
	if (iterClientsNick != _clientsNick.end())
		_clientsNick.erase(iterClientsNick);

	std::map<int, Client *>::iterator			iterClients = _clients.find(client.getSocket());
	if (iterClients == _clients.end())
		std::cerr << "This message is Error in QUIT" << std::endl;
	delete iterClients->second;
	_clients.erase(iterClients);
}
