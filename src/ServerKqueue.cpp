
#include "Server.hpp"
#include "Client.hpp"
#include "Types.hpp"

void Server::makeKqueueReady() {

	_kqueue = kqueue();
	if (_kqueue == FAIL) {
		std::cerr << "error: cannot make kqueue" << std::endl;
		// 서버 종료 함수
		std::exit(EXIT_FAILURE);
	}

	std::memset(&_eventSetting, 0, sizeof(_eventSetting));
	EV_SET(&_eventSetting, _servSocket, EVFILT_READ, EV_ADD | EV_ENABLE , 0, 0, NULL);

	int ret = kevent(_kqueue, &_eventSetting, 1, NULL, 0, NULL);
	if (ret == FAIL) {
		std::cerr << "error: cannot register event on kqueue" << std::endl;
		// kque, socket닫기
		std::exit(1);
	}
}

void Server::addClientKq(int clientSocket) {

	std::memset(&_eventSetting, 0, sizeof(_eventSetting));
	EV_SET(&_eventSetting, clientSocket, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);

	int ret = kevent(_kqueue, &_eventSetting, 1, NULL, 0, NULL);
	if (ret == FAIL) {
		std::cerr << "error: cannot register event on kqueue" << std::endl;
		// kque, socket닫기
		std::exit(1);
	}
}

void Server::removeClientKq(int clientSocket) {

	std::memset(&_eventSetting, 0, sizeof(_eventSetting));
	EV_SET(&_eventSetting, clientSocket, EVFILT_READ, EV_DISABLE, 0, 0, NULL);

	int ret = kevent(_kqueue, &_eventSetting, 1, NULL, 0, NULL);
	if (ret == FAIL) {
		std::cerr << "error: cannot register event on kqueue" << std::endl;
		// kque, socket닫기
		std::exit(1);
	}
}
