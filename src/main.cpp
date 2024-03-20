
#include <csignal>
#include "Server.hpp"
#include "Types.hpp"

int main(int ac, char **av) {

	std::ios_base::sync_with_stdio();
	std::cin.tie(0);
	std::cout.tie(0);

	if (ac != 3) {
		std::cerr << "usage: ./ircserv <port> <password>" << std::endl;
		exit(1);
	}

	Server ircServer(av[PORT], av[PASSWORD]);
	ircServer.runServer();
}
