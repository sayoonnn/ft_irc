
#pragma once

#include <string>

class Client {

	private:
		int _socket;
		std::string _nickname;
		std::string _username;
		std::string _realname;

		std::string _buffer;


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
		std::string getBuffer() const;

		void setSocket(int);
		void setNickname(std::string);
		void setUsername(std::string);
		void setRealname(std::string);
		void setBuffer(std::string);

		void clearBuffer();
		void addToBuffer(std::string);
		bool isBufferEndNl();

};
