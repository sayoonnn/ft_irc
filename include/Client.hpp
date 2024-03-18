
#pragma once

#include <string>

class Client {

	private:
		int _socket;
		std::string _nickname;
		std::string _username;
		std::string _realname;

		bool _isPassed;

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
		bool isPassed() const;

		void setSocket(int);
		void setNickname(std::string);
		void setUsername(std::string);
		void setRealname(std::string);
		void setBuffer(std::string);
		void setPassed();

		void clearBuffer();
		void addToBuffer(std::string);
		bool isBufferEndNl();
		
		bool isRegistered();

};
