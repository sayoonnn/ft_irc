
#include "Server.hpp"
#include "RPL.hpp"


void	Server::PRIVMSG(std::deque<std::string> &parsedCmd, Client &client)
{
	int			fd = client.getSocket();
	char		type = ' ';
	std::string	send;
	std::string	target;

	if (parsedCmd.size() < 2)
	{
		sendMessageToClient(fd, ERR_NEEDMOREPARAMS(client.getNickname(), "PRIVMSG"));
		return ;
	}
	if (parsedCmd.size() < 3)
	{
		sendMessageToClient(fd, ERR_NOTEXTTOSEND(client.getNickname()));
		return ;
	}

	target = parsedCmd[1];
	
	if (target.find(",") != std::string::npos)
		target = target.substr(0, target.find(","));

	type = target[0];

	send = RPL_PRIVMSG(client.getClientInfo(), target, parsedCmd[2]);
	if (type == '#')
	{
		std::map<std::string, Channel *>::iterator	chaIter = _channels.find(target);
		if (chaIter == _channels.end())
		{
			sendMessageToClient(fd, ERR_NOSUCHNICK(client.getNickname(), target));
			return ;
		}
		
		Channel *channel = chaIter->second;
		std::list<int>	fds;
		fds.push_back(fd);
		sendMessageToChannel(*channel, send, fds);
	}
	else
	{
		std::map<std::string, Client *>::iterator cliIter = _clientsNick.find(target);
		if (cliIter == _clientsNick.end())
		{
			sendMessageToClient(fd, ERR_NOSUCHNICK(client.getNickname(), target));
			return ;
		}
		Client *other = cliIter->second;
		sendMessageToClient(other->getSocket(), send);
	}
}

