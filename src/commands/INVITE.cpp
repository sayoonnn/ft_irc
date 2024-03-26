
#include "Server.hpp"
#include "RPL.hpp"

void Server::INVITE(std::deque<std::string> &parsedCmd, Client &client) {
	// param : INVITE nickname #channel
	if (parsedCmd.size() < 3) {
		sendMessageToClient(client.getSocket(), ERR_NEEDMOREPARAMS(client.getNickname(), "INVITE"));
		return ;
	}
	// 확인해보니까 여기선 채널명 검증을 안하더라
	std::string channelName = parsedCmd[2];
	// ',' 가 있으면, 쉼표를 제외한 앞부분만 채널명으로 사용
	if (channelName.find(",") != std::string::npos)
		channelName = channelName.substr(0, channelName.find(","));
	
	// 1. check if channel exist
	if (_channels.find(channelName) == _channels.end()) {
		sendMessageToClient(client.getSocket(), ERR_NOSUCHCHANNEL(client.getNickname(), channelName));
		return ;
	}
	
	// 2. check if client is in channel
	if (_channels[channelName]->isClientIn(client.getSocket()) == 0) {
		sendMessageToClient(client.getSocket(), ERR_NOTONCHANNEL(client.getNickname(), channelName));
		return ;
	}
	
	// // 3. check if client is operator
	// if (_channels[channelName]->getOpers().find(client.getSocket()) == _channels[channelName]->getOpers().end()) {
	// 	sendMessageToClient(client.getSocket(), ERR_CHANOPRIVSNEEDED(client.getNickname(), channelName));
	// 	return ;
	// }
	
	std::string nickname = parsedCmd[1];
	// 4. check if nickname to be invited to channel exists in server
	std::map<int, Client *>::iterator it = _clients.begin();
	for (; it != _clients.end(); it++) {
		if ((*it).second->getNickname() == nickname)
			break ;
	}
	if (it == _clients.end()) {
		sendMessageToClient(client.getSocket(), ERR_NOSUCHNICK(client.getNickname(), nickname));
		return ;
	}
	Client &nickClient = *(*it).second;
	// 5. check if nickClient is already in channel and
	// put nickClient in _invite if nickClient is never invited
	int putInviteResult = _channels[channelName]->putInvite(nickClient.getSocket());
	if (putInviteResult == 0) {
		sendMessageToClient(client.getSocket(), ERR_USERONCHANNEL(client.getNickname(), nickname, channelName));
		return ;
	} else if (putInviteResult == 1)
		nickClient.getInvited().push_back(channelName);
	// 6. invite nickname to channel
	sendMessageToClient(client.getSocket(), RPL_INVITING(client.getNickname(), nickname, channelName));
	sendMessageToClient(client.getSocket(), ":" + client.getNickname() + "!" + client.getUsername() + "@localhost INVITE " + nickname + " :" + channelName + "\n");
}
