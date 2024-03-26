#ifndef CHANNEL_HPP
# define CHANNEL_HPP

#include <iostream>
#include <map>
#include <deque>
#include <algorithm>
#include "Client.hpp"

#define TOPIC_SIZE 100
#define MAX_CLIENTS 50

class Client;

class Channel
{
	private:
		std::string					_name;
		std::string					_key;
		std::string					_topic;
		std::string					_whoTopic; //new
		std::time_t					_topicTimestamp; //new

		bool						_i;
		bool						_t;
		int							_maxClient;

		std::map<int, Client*>	_users;
		std::deque<int>			_invite;
		std::map<int, Client*>	_operators;

		Channel();
		Channel(const Channel& cha);
		Channel& operator=(const Channel& cha);
		
	public:
		Channel(const std::string& na);
		Channel(const std::string& na, int fd, Client& cli);
		~Channel();

		int								putUsers(int fd, Client& cli);
		int								delUsers(int fd);
		int								numUsers() const;
		const std::map<int, Client*>	getUsers() const;
		std::string						getUsersList() const;

		int								putOpers(int fd);
		int								delOpers(int fd);
		int								numOpers() const;
		const std::map<int, Client*>	getOpers() const;

		int								isClientIn(int fd) const;
		int								numClients() const;

		void							setKey(const std::string& k);
		std::string						getKey() const;

		void							setTopic(std::string& to, std::string& who); //fix
		std::string						getTopic() const;
		std::string						getWhoTopic() const; //new
		const std::time_t&				getTimeTopic() const; //new

		int								getMaxNumClients() const;
		void							setMaxNumClients(int l);

		bool							getT() const;
		int								changeT(const char ch);

		bool							getI() const;
		int								changeI(const char ch);

		int								putInvite(int fd);
		int								delInvite(int fd);
		int								isInvite(int fd) const;

		std::string						getName() const;
};

#endif
