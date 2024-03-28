
#include <sstream>
#include <string>
#include <deque>

namespace util {
	bool isAlNum(std::string str);
	bool isPosNum(std::string str);
	void parseCommand(std::string str, std::deque<std::string> &parsedCmd);
	void parseByChar(std::string target, char delimeter, std::deque<std::string> &commands);
}

