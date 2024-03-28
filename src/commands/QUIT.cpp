
#include "Server.hpp"
#include "RPL.hpp"


void Server::QUIT(std::deque<std::string> &parsedCmd, Client &client) {

	std::string	nickname = "Unknown";
	std::string	username = "Unknown";
	std::string	ipAddr = client.getIpAddr();
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
			sendMessageToChannel(*cha, RPL_QUIT(client.getClientInfo(), reason), fds);
	}
	
	if (reason != "EOF from client")
		sendMessageToClient(client.getSocket(), ERR_CLOSELINK(nickname, username, ipAddr, reason));
}
