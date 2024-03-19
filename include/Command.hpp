
#pragma once

#include "Client.hpp"
#include <deque>
#include <map>

#define CMD_NOT_FOUND -1

class Command {
	private:
		typedef	void (Command::*commandFunc)(std::deque<std::string>&, Client &);
		std::map<std::string, commandFunc> _cmdMap;

		Command(const Command &copy);
		Command &operator=(const Command &copy);

	public:
		Command();
		~Command();

		void excuteCommands(Client&);

		void compactSpace(std::string&);
		void parseByString(std::string, std::string, std::deque<std::string>&);

		void PASS(std::deque<std::string>&, Client &);
		void NICK(std::deque<std::string>&, Client &);
		void USER(std::deque<std::string>&, Client &);

		void sendError(int, Client &);
		
};
