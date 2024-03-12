
#include "Server.hpp"
#include "Types.hpp"

int main(int ac, char **av) {

	if (ac != 3) {
		std::cerr << "usage: ./ircserv <port> <password>" << std::endl;
		exit(1);
	}

	Server ircServer(av[PORT], av[PASSWORD]);
	ircServer.runServer();
}
