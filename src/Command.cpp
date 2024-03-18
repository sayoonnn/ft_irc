
#include "Command.hpp"

Command::Command() {
	// _cmdMap["PASS"] = &Command::PASS;
	_cmdMap["NICK"] = &Command::NICK;
	// _cmdMap["USER"] = &Command::USER;
}

Command::~Command() {}

int	Command::isValidCmdFormat(std::deque<std::string> &parsedCmd) {

	size_t paramNeed;
	std::string cmdType;

	cmdType = parsedCmd[0];

	if (cmdType == "QUIT" || cmdType == "MODE" || cmdType== "PONG")
		paramNeed = 1;
	else if (cmdType == "PASS" || cmdType == "NICK" || cmdType == "PART" || cmdType == "JOIN" || cmdType  == "PRIVMSG" || cmdType  == "TOPIC" || cmdType == "PING" || cmdType == "PART")
		paramNeed = 2;
	else if (cmdType == "KICK" || cmdType == "INVITE")
		paramNeed = 3;
	else if (cmdType == "USER")
		paramNeed = 5;
	else
		return (CMD_NOT_FOUND);

	return (paramNeed == parsedCmd.size());
}


void Command::parseByString(std::string target, std::string delimeter, std::deque<std::string> &commands) {

	size_t start = 0;
	size_t loc = target.find(delimeter, start);

	while (loc != std::string::npos) {
		commands.push_back(target.substr(start, loc));
		start = loc + 1;
		loc = target.find("\n", start);
	}

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

		if (client.isRegistered()) {
			errCode = isValidCmdFormat(parsedCmd);
			if (errCode == -1)
				sendError(461, client);
			else if (errCode == 0)
				sendError(421, client);
			else if (parsedCmd[0] == "USER" || parsedCmd[0] == "PASS")
				sendError(462, client);
			else
				(this->*_cmdMap[cmdType])(parsedCmd, client);
		}
		else {
			cmdType = parsedCmd[0];
			errCode = isValidCmdFormat(parsedCmd);
			if (!errCode &&
			(cmdType == "USER" || cmdType == "NICK" || cmdType == "PASS")) {
				if (errCode == -1)
					sendError(461, client);
				else
					(this->*_cmdMap[cmdType])(parsedCmd, client);
			}
			else
				sendError(451, client);
		}

		parsedCmd.clear();
	}

	client.clearBuffer();
}

void Command::sendError(int errNum, Client &client) { (void) errNum, (void) client;}

void Command::NICK(std::deque<std::string> &parsedCmd, Client &client) {

	client.setNickname(parsedCmd[1]);
}

void Command::PASS(std::deque<std::string> &parsedCmd, Client &client) {

	client.setPassed();
}

void Command::USER(std::deque<std::string> &parsedCmd, Client &client) {

	client.setUsername(parsedCmd[1]);
}

