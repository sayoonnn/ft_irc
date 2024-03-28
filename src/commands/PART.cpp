
#include "Server.hpp"
#include "RPL.hpp"

void	Server::PART(std::deque<std::string> &parsedCmd, Client &client)
{
	std::map<std::string, Channel *>::iterator chaIter;

	if (parsedCmd.size() < 2)
	{
		sendMessageToClient(client.getSocket(), ERR_NEEDMOREPARAMS(client.getNickname(), "PART"));
		return ;
	}

	std::string	chaName = parsedCmd[1];
	
	if (chaName.find(",") != std::string::npos)
		chaName = chaName.substr(0, chaName.find(","));

	chaIter = _channels.find(chaName);
	if (chaIter == _channels.end())
	{
		sendMessageToClient(client.getSocket(), ERR_NOSUCHCHANNEL(client.getNickname(), chaName));
		return ;
	}
	Channel* channel = chaIter->second;
	if (channel->delUsers(client.getSocket()) == 0)
	{
		sendMessageToClient(client.getSocket(), ERR_NOTONCHANNEL(client.getNickname(), chaName));
		return ;
	}
	client.partChannel(parsedCmd[1]);

	std::string	reason = "";

	if (parsedCmd.size() >= 3)
		reason = parsedCmd[2];
	sendMessageToClient(client.getSocket(), RPL_PART(client.getNickname(), client.getUsername(), chaName, reason));
	if (isNoClientInChannel(parsedCmd[1]) == 0)
		sendMessageToChannel(*channel, RPL_PART(client.getNickname(), client.getUsername(), chaName, reason));
}
