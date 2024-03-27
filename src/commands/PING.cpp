
#include "Server.hpp"
#include "RPL.hpp"

void Server::PING(std::deque<std::string> &parsedCmd, Client &client) {

	if (parsedCmd.size() < 2) {
		sendMessageToClient(client.getSocket(), ERR_NEEDMOREPARAMS(client.getNickname(), "PING"));
		return ;
	}

	sendMessageToClient(client.getSocket(), RPL_PONG(parsedCmd[1]));
}

