
#include "Server.hpp"
#include "RPL.hpp"

void	Server::TOPIC(std::deque<std::string> &parsedCmd, Client &client)
{
	std::map<std::string, Channel *>::iterator	chaIter;
	int			fd = client.getSocket();
	Channel*	channel;
	std::string	send;

	if (parsedCmd.size() < 2)
	{
		sendMessageToClient(fd, ERR_NEEDMOREPARAMS(client.getNickname(), "TOPIC"));
		return ;
	}

	chaIter = _channels.find(parsedCmd[1]);
	if (chaIter == _channels.end())
	{
		sendMessageToClient(fd, ERR_NOSUCHCHANNEL(client.getNickname(), parsedCmd[1]));
		return ;
	}

	channel = chaIter->second;
	if (channel->isClientIn(fd) == 0)
	{
		sendMessageToClient(fd, ERR_NOTONCHANNEL(client.getNickname(), parsedCmd[1]));
		return ;
	}

	if (parsedCmd.size() >= 3)
	{
		if (channel->getT() == true && channel->isClientIn(fd) == 1)
		{
			sendMessageToClient(fd, ERR_CHANOPRIVSNEEDED(client.getNickname(), parsedCmd[1]));
			return ;
		}
		channel->setTopic(parsedCmd[2], send);
		sendMessageToChannel(*channel, RPL_CHANGETOPIC(client.getNickname(), client.getUsername(), parsedCmd[1], parsedCmd[2]));
	}
	else
	{
		if (channel->getTopic() == "")
		{
			sendMessageToClient(fd, RPL_NOTOPIC(client.getNickname(), parsedCmd[1]));
			return ;
		}
		sendMessageToClient(fd, RPL_TOPIC(client.getNickname(), parsedCmd[1], channel->getTopic()));
		sendMessageToClient(fd, RPL_TOPICWHOTIME(client.getNickname(), parsedCmd[1], channel->getWhoTopic(), channel->getTimeTopic()));
	}
}

