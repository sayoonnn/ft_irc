
#include "Server.hpp"
#include "RPL.hpp"

#define NOT_IN 0
#define USER 1

void	Server::KICK(std::deque<std::string> &parsedCmd, Client &client)
{
	std::map<std::string, Channel *>::iterator	chaIter;
	std::map<std::string, Client *>::iterator	cliIter;
	int			fd = client.getSocket();
	Client*		kickClient;
	Channel*	channel;
	std::string	send;

	if (parsedCmd.size() < 3)
	{
		sendMessageToClient(fd, ERR_NEEDMOREPARAMS(client.getNickname(), "KICK"));
		return ;
	}

	chaIter = _channels.find(parsedCmd[1]);
	if (chaIter == _channels.end())
	{
		sendMessageToClient(fd, ERR_NOSUCHCHANNEL(client.getNickname(), parsedCmd[1]));
		return ;
	}

	channel = chaIter->second;
	if (channel->isClientIn(fd) == NOT_IN)
	{
		sendMessageToClient(fd, ERR_NOTONCHANNEL(client.getUsername(), client.getNickname(), parsedCmd[1]));
		return ;
	}

	if (channel->isClientIn(fd) == USER)
	{
		sendMessageToClient(fd, ERR_CHANOPRIVSNEEDED(client.getNickname(), parsedCmd[1]));
		return ;
	}

	cliIter = _clientsNick.find(parsedCmd[2]);
	if (cliIter == _clientsNick.end())
	{
		sendMessageToClient(fd, ERR_NOSUCHNICK(client.getNickname(), parsedCmd[2]));
		return ;
	}

	kickClient = cliIter->second;
	if (channel->delUsers(kickClient->getSocket()) == 0)
	{
		sendMessageToClient(fd, ERR_USERNOTINCHANNEL(client.getUsername(), client.getNickname(), channel->getName()));
		return ;
	}
	//kickclient의 joined에서 대상 채널을 제거하는 함수 추가해야함.

	send = ":" + client.getNickname() + "!" + client.getUsername() + "@localhost KICK " + parsedCmd[1] + " " + parsedCmd[2] + " :";
	if (parsedCmd.size() >= 4)
		send += parsedCmd[3];
	sendMessageToClient(kickClient->getSocket(), send);
	if (isNoClientInChannel(parsedCmd[1]) == 0)
		sendMessageToChannel(*channel, send);
}

