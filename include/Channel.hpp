#ifndef CHANNEL_HPP
# define CHANNEL_HPP

#include <iostream>
#include <map>

#define TOPIC_SIZE 10
#define MAX_CLIENTS 3

class Client {};

class Channel
{
	private:
		std::string					key;
		std::string					name;
		std::string					topic;

		int							max_num_clients;
		bool						i;
		bool						t;

		std::map<int, Client*>	users;
		std::map<int, Client*>	invite;
		std::map<int, Client*>	operators;

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
		const std::map<int, Client*>&	getUsers() const;

		int								putOpers(int fd);
		int								delOpers(int fd);
		int								numOpers() const;
		const std::map<int, Client*>&	getOpers() const;

		int								isClientIn(int fd) const;
		int								numClients() const;

		void							setKey(const std::string& k);
		std::string						getKey() const;

		void							setTopic(const std::string& to);
		std::string						getTopic() const;

		int								getMaxNumClients() const;
		void							setMaxNumClients(int l);

		bool							getT() const;
		int								changeT(const char ch);

		bool							getI() const;
		int								changeI(const char ch);

		int								putInvite(int fd, Client& cli);
		int								delInvite(int fd);
		int								isInvite(int fd) const;

		std::string						getName() const;
};

#endif
