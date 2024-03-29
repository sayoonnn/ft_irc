
#include "Server.hpp"
#include "RPL.hpp"

void Server::JOIN(std::deque<std::string> &parsedCmd, Client &client) {
    // param : JOIN #channel [key]   (key is optional)
        // 인자가 2개까지만 처리하도록 하겠음.
	if (parsedCmd.size() < 2) {
		sendMessageToClient(client.getSocket(), ERR_NEEDMOREPARAMS(client.getNickname(), "JOIN"));
		return ;
	}

    // 0. check if channel name is valid : (ex: #channel)
    std::string channelName = parsedCmd[1];
	// ',' 가 있으면, 쉼표를 제외한 앞부분만 채널명으로 사용
	if (channelName.find(",") != std::string::npos)
		channelName = channelName.substr(0, channelName.find(","));
	
	// 채널명이 #으로 시작하지 않거나, 그 다음이 숫자나 영문자로 이루어지지 않으면 에러
	if (channelName[0] != '#') {
		sendMessageToClient(client.getSocket(), ERR_BADCHANMASK(channelName));
		return ;
	}

	for (size_t i = 1; i < channelName.size(); i++) {
		if (!std::isalnum(channelName[i])) {
			sendMessageToClient(client.getSocket(), ERR_BADCHANMASK(channelName));
			return ;
		}
	}

	// 1. check if client joined too many channels
	if (client.getChannels().size() >= MAX_CHANNELS) {
		sendMessageToClient(client.getSocket(), ERR_TOOMANYCHANNELS(client.getNickname(), channelName));
		return ;
	}
	 
	if (_channels.find(channelName) == _channels.end())
		_channels[channelName] = new Channel(channelName, client.getSocket(), client);
	else {
		// 3. check if channel is invite only (+i)
		if (_channels[channelName]->getI() == 1) {
			// check if client is invited
			if (std::find(client.getInvited().begin(), client.getInvited().end(), channelName) == client.getInvited().end()) {
				sendMessageToClient(client.getSocket(), ERR_INVITEONLYCHAN(client.getNickname(), channelName));
				return ;
			}
			// check if Channel::_invite list has client
			if (_channels[channelName]->isInvite(client.getSocket()) == 0) {
				sendMessageToClient(client.getSocket(), ERR_INVITEONLYCHAN(client.getNickname(), channelName));
				return ;
			}
			// remove channelName from Client::_invited list
			client.removeInvitaion(channelName);
			// remove client from Channel::_invite list
			_channels[channelName]->delInvite(client.getSocket());
		}

		// 5. check if channel has key (+k)
		if (_channels[channelName]->getKey().empty() == false) {
			if (parsedCmd.size() < 3) {
				sendMessageToClient(client.getSocket(), ERR_BADCHANNELKEY(client.getNickname(), channelName));
				return ;
			}
			if (parsedCmd[2] != _channels[channelName]->getKey()) {
				sendMessageToClient(client.getSocket(), ERR_BADCHANNELKEY(client.getNickname(), channelName));
				return ;
			}
		}

		int putUsersResult = _channels[channelName]->putUsers(client.getSocket(), client);
		if (putUsersResult == 0)
			return ;
		else if (putUsersResult == -1) {
			sendMessageToClient(client.getSocket(), ERR_CHANNELISFULL(client.getNickname(), channelName));
			return ;
		}
	}

	// 6. join channel
	client.joinChannel(channelName, _channels[channelName]);
	sendMessageToChannel(*_channels[channelName], RPL_JOIN(client.getClientInfo(), channelName));

	// 4. check if channel has topic (+t)
	if (!(_channels[channelName]->getTopic().empty())) {
		sendMessageToClient(client.getSocket(), RPL_TOPIC(client.getNickname(), channelName, _channels[channelName]->getTopic()));
		sendMessageToClient(client.getSocket(), RPL_TOPICWHOTIME(client.getNickname(), channelName, _channels[channelName]->getWhoTopic(), _channels[channelName]->getTimeTopic()));
	}

	// 7. send RPL_NAMREPLY
	sendMessageToClient(client.getSocket(), RPL_NAMREPLY(client.getNickname(), channelName, _channels[channelName]->getUsersList()));
	// 8. send RPL_ENDOFNAMES
	sendMessageToClient(client.getSocket(), RPL_ENDOFNAMES(client.getNickname(), channelName));
}
