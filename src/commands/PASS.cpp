
#include "Server.hpp"
#include "RPL.hpp"

bool Server::PASS(std::deque<std::string> &parsedCmd, Client &client) {

	if (parsedCmd.size() < 2) {
		sendMessageToClient(client.getSocket(), ERR_NEEDMOREPARAMS(client.getNickname(), "PASS"));
		return (true);
	}

	if (client.isPassed()) {
		sendMessageToClient(client.getSocket(), ERR_ALREADYREGISTERED(client.getNickname()));
		return (true);
	}

	if (_password != parsedCmd[1]) {
		sendMessageToClient(client.getSocket(), ERR_PASSWDMISMATCH(client.getNickname()));
		sendMessageToClient(client.getSocket(), RPL_ERROR(std::string("connection refused")));
		return (false);
	}

	client.setPassed();
	return (true);
}
