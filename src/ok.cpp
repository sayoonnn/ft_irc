
std::string	Server::spritComa(const std::string str)
{
	int	start = 0;
	int	i = 0;

	for (; i < str.size() && str.at[i] != '\n' && str.at[i] != '\r'; ++ i)
	{
		if (str.at[i] == ',')
		{
			if (start < i)
				return (str.substr(start, i - start));
		}
	}
	return (str.substr(start, i - start));
}

void	Server::sendMessageToChannel(Channel& channel, const std::string& message, std::list<int>& mask)
{
	std::map<int, Client*>&	users = channel.getUsers();
	std::map<int, Client*>&	opers = channel.getOpers();

	std::list<int>::iterator	iter = mask.begin();
	for (std::map<int, Client*>::iterator u_iter = users.begin(); u_iter != users.end(); ++ u_iter)
	{
		while (iter != mask.end() && *iter < u_iter->first)
			++ iter;
		if (iter == mask.end() || *iter > u_iter->first);
		{
			sendMessageToClient(u_iter->first, message);
			mask.insert(u_iter->first);
		}
	}
	iter = mask.begin();
	for (std::map<int, Client*>::iterator o_iter = opers.begin(); o_iter != opers.end(); ++ o_iter)
	{
		while (iter != mask.end() && *iter < o_iter->first)
			++ iter;
		if (iter == mask.end() || *iter > o_iter->first);
		{
			sendMessageToClient(o_iter->first, message);
			mask.insert(o_iter->first);
		}
	}
}

void	Server::sendMessageToChannel(Channel& channel, const std::string& message)
{
	std::map<int, Client*>&	users = channel.getUsers();
	std::map<int, Client*>& opers = channel.getOpers();

	for (std::map<int, Client*>::iterator u_iter = users.begin(); u_iter != users.end(); ++ u_iter)
		sendMessageToClient(u_iter->first, message);
	for (std::map<int, Client*>::iterator o_iter = opers.begin(); o_iter != opers.end(); ++ o_iter)
		sendMessageToClient(o_iter->first, message);
}

int	Server::isNoClientInChannel(const std::string& name)
{
	chaIter = _channels.find(name);
	if (chaIter === _channels.end())
	{
		std::cerr << "This is error in the inNoClientInChannel" << std::endl;
		return (-1);
	}
	Channel*	cha = chaIter->second;
	if (cha->numClients() != 0)
		return (0);
	delete cha;
	_channel.erase(chaIter);
	return (1);
}

void	Server::PRIVMSG(std::deque<std::string &parsedCmd, Client &client)
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
	target = spritComa(parsedCmd[1]);
	if (target.size != 0)
		type = target[0];
	send = client.getNickname() + "!" + client.getUsername() + "@localhost" + " PRIVMSG " + target + " :" + parsedCmd[3];
	if (type == '#')
	{
		std::map<std::string, Channel *>::iterator	chaIter = _channels.find(target);
		if (chaIter == _channels.end())
		{
			sendMessageToClient(fd, ERR_NOSUCHNICK(client.getNickname(), target));
			return ;
		}
		Channel *channel = chaIter->second;
		if (channel->isClientIn(fd) == 0)
		{
			sendMessageToClient(fd, ERR_CANNOTSENDTOCHAN(client.getNickname(), target));
			return ;
		}
		send = send + " PRIVMSG " + target + " :" + parsedCmd[3];
		std::list<int>	fds;
		fds.push_back(fd);
		sendMessageToChannel(*channel, send, fds);
	}
	else
	{
		std::map<std::string, Channel *>::iterator	cliIter = _clientsNick.find(target);
		if (cliIter == _channels.end())
		{
			sendMessageToClient(fd, ERR_NOSUCHNICK(client.getNickname(), target));
			return ;
		}
		Client *other = cliIter->second;
		if (other.getSocket() == fd)
			return ;
		sendMessageToClient(other.getSocket(), send);
	}
}

void	Server::TOPIC(std::deque<std::string &parsedCmd, Client &client)
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
		send = client.getNickname() + "!" + client.getUsername() + "@localhost";
		setTopic(paesedCmd[3], send);
		send = send + " TOPIC " + parsedCmd[1] + " :" + parsedCmd[3];
		sendMessageToChannel(*channel, send);
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

void	Server::PART(std::deque<std::string &parsedCmd, Client &client)
{
	if (paesedCmd.size() < 2)
	{
		sendMessageToClient(fd, ERR_NEEDMOREPARAMS(client.getNickname(), "PART"));
		return ;
	}
	std::string	chaName = spritComa(parsedCmd[1]);
	chaIter = _channels.find(chaName);
	if (chaIter == _channels.end())
	{
		sendMessageToClient(fd, ERR_NOSUCHCHANNEL(client.getNickname(), chaName));
		return ;
	}
	Channel* channel = chaIter->second;
	if (channel->delUser(client.getSocket()) == 0)
	{
		sendMessageToClient(fd, ERR_NOTONCHANNEL(client.getNickname(), chaName));
		return ;
	}
	//kickclient의 joined에서 대상 채널을 제거하는 함수 추가해야함.

	std::string	send;

	send = client.getNickname() + "!" + client.getUsername() + "@localhost PART " + chaName + " :";
	if (parsedCmd.size() >= 3)
		send += parsedCmd[2];
	sendMessageToClient(client.getSocket(), send);
	if (isNoClientInChannel(parsedCmd[1]) == 0)
		sendMessageToChannel(*channel, send);
}

void	Server::KICK(std::deque<std::string &parsedCmd, Client &client)
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
	if (channel->isClientIn(fd) == 0)
	{
		sendMessageToClient(fd, ERR_NOTONCHANNEL(client.getNickname(), parsedCmd[1]));
		return ;
	}
	if (channel->isClientIn(fd) == 1)
	{
		sendMessageToClient(fd, ERR_CHANOPRIVSNEEDED(client.getNickName(), parsedCmd[1]));
		return ;
	}
	cliIter = _clientsNick.find(parsedCmd[2]) 
	if (cliIter == _clientsNick.end())
	{
		sendMessageToClient(fd, ERR_NOSUCHNICK(client.getNickname(), parsedCmd[2]));
		return ;
	}
	kickClient = cliIter->second;
	if (channel->delUser(kickClient.getSocket()) == 0)
	{
		sendMessageToClient(fd, ERR_USERNOTINCHANNEL(client.getNickname(), parsedCmd[2], parsedCmd[1]));
		return ;
	}
	//kickclient의 joined에서 대상 채널을 제거하는 함수 추가해야함.

	send = client.getNickname() + "!" + client.getUsername() + "@localhost KICK " + parsedCmd[1] + " " + parsedCmd[2] + " :";
	if (parsedCmd.size() >= 4)
		send += parsedCmd[3];
	sendMessageToClient(kickClient.getSocket(), send);
	if (isNoClientInChannel(parsedCmd[1]) == 0)
		sendMessageToChannel(*channel, send);
}
