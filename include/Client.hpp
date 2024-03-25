
#pragma once

#include "Channel.hpp"
#include <map>
#include <string>

#define MAX_CHANNELS 50

class Channel;

class Client {

	private:
		int _socket;
		std::string _nickname;
		std::string _username;
		std::string _realname;
		std::map<std::string, Channel *> _channels;

		bool _isPassed;
		bool _isRegistered;

		std::string _buffer;

		std::deque<std::string>	_invited;

	public:
		Client();
		Client(int);
		~Client();
		Client(const Client &copy);
		Client &operator=(const Client &copy);

		int getSocket() const;
		std::string getNickname() const;
		std::string getUsername() const;
		std::string getRealname() const;
		std::map<std::string, Channel *>	getChannels() const;
		std::deque<std::string>				getInvited() const;
		std::string getBuffer() const;
		bool isPassed() const;

		void setSocket(int);
		void setNickname(std::string);
		void setUsername(std::string);
		void setRealname(std::string);
		void setBuffer(std::string);
		void setPassed();
		void setRegistered();

		void clearBuffer();
		void addToBuffer(std::string);
		bool isBufferEndNl();
		
		bool isRegistered();

};
