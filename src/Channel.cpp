#include "Channel.hpp"

Channel::Channel(const std::string& name)
: _name(name), _key(""), _topic(""), _i(false), _t(false), _maxClient(MAX_CLIENTS) {}

Channel::Channel(const std::string& name, int clientSocket, Client& client)
: _name(name), _key(""), _topic(""), _i(false), _t(false), _maxClient(MAX_CLIENTS)
{
	_operators.insert(std::pair<int, Client*>(clientSocket, &client));
}

Channel::~Channel() {}

//    Users

int	Channel::putUsers(int fd, Client& cli)
{
	if (isClientIn(fd) != 0)
		return (0);
	if (numClients() >= _maxClient)
		return (-1);
	delInvite(fd);
	_users.insert(std::pair<int, Client*>(fd, &cli));
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
			iter = _users.find(fd);
			_users.erase(iter);
			break;
		case 2:
			iter = _operators.find(fd);
			_operators.erase(iter);
	}
	return (1);
}

int	Channel::numUsers() const
{
	return (_users.size());
}

const std::map<int, Client*>	Channel::getUsers() const
{
	return (_users);
}

std::string	Channel::getUsersList() const
{
	std::string	list;

	for (std::map<int, Client*>::const_iterator it = _users.begin(); it != _users.end(); it++)
		list += it->second->getNickname() + " ";
	for (std::map<int, Client*>::const_iterator it = _operators.begin(); it != _operators.end(); it++)
		list += "@" + it->second->getNickname() + " ";
	return (list);
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

	iter = _users.find(fd);
	_operators.insert(std::pair<int, Client*>(fd, iter->second));
	_users.erase(iter);
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

	iter = _operators.find(fd);
	_users.insert(std::pair<int, Client*>(fd, iter->second));
	_operators.erase(iter);
	return (1);
}

int	Channel::numOpers() const
{
	return (_operators.size());
}

const std::map<int, Client*>	Channel::getOpers() const
{
	return (_operators);
}

//    Clients

int	Channel::isClientIn(int fd) const
{
	if (_users.find(fd) != _users.end())
		return (1);
	if (_operators.find(fd) != _operators.end())
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
	_key = k;
}

std::string	Channel::getKey() const
{
	return (_key);
}

//    Topic

void	Channel::setTopic(std::string& to, std::string& who)
{
	if (to.size() > TOPIC_SIZE)
		_topic = to.substr(0, TOPIC_SIZE);
	else
		_topic = to;
	_whoTopic = who;

	time_t tmp = std::time(0);
	_topicTimestamp = std::ctime(&tmp);
}

std::string	Channel::getTopic() const
{
	return (_topic);
}

std::string	Channel::getWhoTopic() const
{
	return (_whoTopic);
}

std::string	Channel::getTimeTopic() const
{
	return (_topicTimestamp);
}

//    MaxNumClients

int	Channel::getMaxNumClients() const
{
	return (_maxClient);
}

void	Channel::setMaxNumClients(int l)
{
	_maxClient = l;
}

//    T

bool	Channel::getT() const
{
	return (_t);
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
	if (ok == _t)
		return (0);
	_t = ok;
	return (1);
}

//    I

bool	Channel::getI() const
{
	return (_i);
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
	if (ok == _i)
		return (0);
	_i = ok;
	return (1);
}

//    Invite

int	Channel::delInvite(int fd)
{
	if (isInvite(fd) == 0)
		return (0);
	_invite.erase(std::remove(_invite.begin(), _invite.end(), fd), _invite.end());
	return (1);
}

int	Channel::putInvite(int fd)
{
	if (isClientIn(fd) != 0)
		return (0);
	if (isInvite(fd) != 0)
		return (-1);
	_invite.push_back(fd);
	return (1);
}

int	Channel::isInvite(int fd) const
{
	if (std::find(_invite.begin(), _invite.end(), fd) != _invite.end())
		return (1);
	return (0);
}

//    name

std::string	Channel::getName() const
{
	return (_name);
}
