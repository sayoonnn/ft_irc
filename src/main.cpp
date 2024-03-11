
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <iostream>
#include <string>

#include <vector>
#include <sstream>

#define MAXPENDING 5

void handleCapLs(int clientSocket) {
    // 클라이언트에게 지원되는 CAP 목록을 전송
    std::string capList = "CAP * LS :multi-prefix sasl\r\n";
    send(clientSocket, capList.c_str(), capList.size(), 0);
}

void handleCapReq(int clientSocket) {

    // CAP END 메시지 전송
    std::string capEnd = "CAP * END\r\n";
    send(clientSocket, capEnd.c_str(), capEnd.size(), 0);
}

int main(int ac, char **av) {

	if (ac != 3) {
		std::cerr << "usage: ./ircserv <port> <password>" << std::endl;
		exit(1);
	}

	int socketFd;
	int servPort = std::atoi(av[1]);
	struct sockaddr_in servAddr;

	if ((socketFd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1) {
		std::cerr << "cannot open stream socket!" << std::endl;
		exit(1);
	}

	servAddr.sin_family = AF_INET;
	servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servAddr.sin_port = htons(servPort);

	bind(socketFd, (struct sockaddr *)&servAddr, sizeof(servAddr));
	listen(socketFd, SOMAXCONN);

	while (true) {
		struct sockaddr_in clntAddr;
		u_int clntLen = sizeof(clntAddr);
		int clntSock;
		char echoBuffer[1024];

		clntSock = accept(socketFd, (struct sockaddr *) &clntAddr, &clntLen);
		recv(clntSock, echoBuffer, 1024, 0);

		std::cout << echoBuffer << std::endl;

		handleCapLs(clntSock);
		std::cout << "send!" << std::endl;
	}

}



