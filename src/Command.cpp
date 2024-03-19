
#include "Command.hpp"
#include <iostream>

Command::Command() {
	_cmdMap["PASS"] = &Command::PASS;
	_cmdMap["NICK"] = &Command::NICK;
	_cmdMap["USER"] = &Command::USER;
}

Command::~Command() {}

void Command::parseByString(std::string target, std::string delimeter, std::deque<std::string> &commands) {

	size_t start = 0;
	size_t loc = target.find(delimeter, start);

	while (loc != std::string::npos) {
		commands.push_back(target.substr(start, loc - 1));
		start = loc + 1;
		loc = target.find("\n", start);
	}

	if (start != target.size())
		commands.push_back(target.substr(start, target.size()));
}

void  Command::compactSpace(std::string& str) {

	size_t start;
	size_t loc;

	start = 0;
	loc = str.find("  ", start);

	while (loc != std::string::npos) {
		int cnt = 0;

		while (str[loc + 1 + cnt] == ' ')
			cnt++;

		str.erase(loc + 1, cnt);
		start = loc + 1;
		loc = str.find("  ", start);
	}

	if (str[0] == ' ')
		str.erase(0, 1);
}

void Command::excuteCommands(Client& client)
{
	std::deque<std::string>		commands;
	std::deque<std::string>		parsedCmd;
	std::string					cmdType;
	int							errCode;

	parseByString(client.getBuffer(), "\n", commands);

	for (size_t i = 0; i < commands.size(); i++) {

		compactSpace(commands[i]);
		parseByString(commands[i], " ", parsedCmd);
		cmdType = parsedCmd[0];

		if (_cmdMap.find(cmdType) == _cmdMap.end())
			sendError(421, client);
		else if (cmdType == "PASS" || client.isRegistered())
			(this->*_cmdMap[cmdType])(parsedCmd, client);
		else
			sendError(451, client);

		parsedCmd.clear();
	}

	client.clearBuffer();
}

void Command::sendError(int errNum, Client &client) { 
	(void) errNum, (void) client;
	std::cout << "error: invalid command" << std::endl;
}

void Command::NICK(std::deque<std::string> &parsedCmd, Client &client) {

	client.setNickname(parsedCmd[1]);
}

void Command::PASS(std::deque<std::string> &parsedCmd, Client &client) {

	(void)parsedCmd;
	client.setPassed();
}

void Command::USER(std::deque<std::string> &parsedCmd, Client &client) {

	client.setUsername(parsedCmd[1]);
}

