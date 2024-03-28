
#include "Utils.hpp"

bool util::isAlNum(std::string str) {

	if (str.size() == 0)
		return (false);

	for (size_t i = 0; i < str.size(); i++) {
		if (!std::isalnum(str[i]))
			return (false);
	}

	return (true);

}

bool util::isPosNum(std::string str) {

	for (size_t i = 1; i < str.size(); i++) {
		if (!std::isdigit(str[i]))
			return (false);
	}

	std::stringstream ss(str);
	int tmp;

	ss >> tmp;
	if (tmp <= 0)
		return (false);

	return (true);
}


void  util::parseCommand(std::string str, std::deque<std::string> &parsedCmd) {

	std::string colonArg;

	if (str.find(":") != std::string::npos) {
		colonArg = str.substr(str.find(":") + 1, str.size());
		str = str.substr(0, str.find(":"));
	}

	std::stringstream ss(str);
	std::string tmp;

	while (ss >> tmp)
		parsedCmd.push_back(tmp);
	
	if (colonArg != "")
		parsedCmd.push_back(colonArg);
}

void util::parseByChar(std::string target, char delimeter, std::deque<std::string> &commands) {

	std::stringstream ss(target);
	std::string tmp;

	while (std::getline(ss, tmp, delimeter))
		commands.push_back(tmp);
}
