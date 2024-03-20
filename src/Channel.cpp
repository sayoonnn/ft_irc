#include "../include/Channel.hpp"

//    private

Channel::Channel()
{
}

Channel::Channel(const Channel& cha)
{
	(void)cha;
}

Channel&	Channel::operator=(const Channel& cha)
{
	(void)cha;
	return (*this);
}

//    public


Channel::Channel(const std::string& na)
{
	name = na;
	key = "";
	topic = "";
	i = false;
	t = false;
	max_num_clients = MAX_CLIENTS;
}

Channel::Channel(const std::string& na, int fd, Client& cli)
{
	name = na;
	key = "";
	topic = "";
	i = false;
	t = false;
	max_num_clients = MAX_CLIENTS;
	operators.insert(std::pair<int, Client*>(fd, &cli));
}

Channel::~Channel()
{
}

//    Users

int	Channel::putUsers(int fd, Client& cli)
{
	if (isClientIn(fd) != 0)
		return (0);
	if (numClients() >= max_num_clients)
		return (-1);
	delInvite(fd);
	users.insert(std::pair<int, Client*>(fd, &cli));
	return (1);
}

int	Channel::delUsers(int fd)
{
	std::map<int, Client*>::iterator	iter;

	switch (isClientIn(fd))
	{
		case 0:
			return (0);
		case 1:
			iter = users.find(fd);
			users.erase(iter);
			break;
		case 2:
			iter = operators.find(fd);
			operators.erase(iter);
	}
	return (1);
}

int	Channel::numUsers() const
{
	return (users.size());
}

const std::map<int, Client*>&	Channel::getUsers() const
{
	return (users);
}

//    Opers

int	Channel::putOpers(int fd)
{
	switch (isClientIn(fd))
	{
		case 0:
			return (0);
		case 2:
			return (-1);
	}
	std::map<int, Client*>::iterator	iter;

	iter = users.find(fd);
	operators.insert(std::pair<int, Client*>(fd, iter->second));
	users.erase(iter);
	return (1);
}

int	Channel::delOpers(int fd)
{
	switch (isClientIn(fd))
	{
		case 0:
			return (0);
		case 1:
			return (-1);
	}
	std::map<int, Client*>::iterator	iter;

	iter = operators.find(fd);
	users.insert(std::pair<int, Client*>(fd, iter->second));
	operators.erase(iter);
	return (1);
}

int	Channel::numOpers() const
{
	return (operators.size());
}

const std::map<int, Client*>&	Channel::getOpers() const
{
	return (operators);
}

//    Clients

int	Channel::isClientIn(int fd) const
{
	if (users.find(fd) != users.end())
		return (1);
	if (operators.find(fd) != operators.end())
		return (2);
	return (0);
}

int	Channel::numClients() const
{
	return (numOpers() + numUsers());
}

//    Key

void	Channel::setKey(const std::string& k)
{
	key = k;
}

std::string	Channel::getKey() const
{
	return (key);
}

//    Topic

void	Channel::setTopic(const std::string& to)
{
	if (to.size() > TOPIC_SIZE)
		topic = to.substr(0, TOPIC_SIZE);
	else
		topic = to;
}

std::string	Channel::getTopic() const
{
	return (topic);
}

//    MaxNumClients

int	Channel::getMaxNumClients() const
{
	return (max_num_clients);
}

void	Channel::setMaxNumClients(int l)
{
	max_num_clients = l;
}

//    T

bool	Channel::getT() const
{
	return (t);
}

int	Channel::changeT(const char ch)
{
	bool	ok;

	if (ch == '-')
		ok = false;
	else if (ch == '+')
		ok = true;
	else
		return (-1);
	if (ok == t)
		return (0);
	t = ok;
	return (1);
}

//    I

bool	Channel::getI() const
{
	return (i);
}

int	Channel::changeI(const char ch)
{
	bool	ok;

	if (ch == '-')
		ok = false;
	else if (ch == '+')
		ok = true;
	else
		return (-1);
	if (ok == i)
		return (0);
	i = ok;
	return (1);
}

//    Invite

int	Channel::delInvite(int fd)
{
	if (isInvite(fd) == 0)
		return (0);
	invite.erase(invite.find(fd));
	return (1);
}

int	Channel::putInvite(int fd, Client& cli)
{
	if (isClientIn(fd) != 0)
		return (0);
	invite.insert(std::pair<int, Client*>(fd, &cli));
	return (1);
}

int	Channel::isInvite(int fd) const
{
	if (invite.find(fd) != invite.end())
		return (1);
	return (0);
}

//    name


std::string	Channel::getName() const
{
	return (name);
}
