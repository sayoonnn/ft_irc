
#include "Server.hpp"
#include "Client.hpp"
#include "Types.hpp"

void Server::makeKqueueReady() {

	_kqueue = kqueue();
	if (_kqueue == FAIL)
		closeServer("cannot open kqueue");

	std::memset(&_eventSetting, 0, sizeof(_eventSetting));
	EV_SET(&_eventSetting, _servSocket, EVFILT_READ, EV_ADD | EV_ENABLE , 0, 0, NULL);

	int ret = kevent(_kqueue, &_eventSetting, 1, NULL, 0, NULL);
	if (ret == FAIL)
		closeServer("cannot register event on kqueue");
}

void Server::addClientKq(int clientSocket) {

	std::memset(&_eventSetting, 0, sizeof(_eventSetting));
	EV_SET(&_eventSetting, clientSocket, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);

	int ret = kevent(_kqueue, &_eventSetting, 1, NULL, 0, NULL);
	if (ret == FAIL)
		closeServer("cannot register event on kqueue");
}

void Server::removeClientKq(int clientSocket) {

	std::memset(&_eventSetting, 0, sizeof(_eventSetting));
	EV_SET(&_eventSetting, clientSocket, EVFILT_READ, EV_DISABLE, 0, 0, NULL);

	int ret = kevent(_kqueue, &_eventSetting, 1, NULL, 0, NULL);
	if (ret == FAIL)
		closeServer("cannot remove event on kqueue");
}
