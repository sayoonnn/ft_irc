
#include "Client.hpp"

Client::Client(): _socket(-1), _nickname(""), _username(""), _realname(""), _isPassed(false), _isRegistered(false) {}

Client::Client(int socket): _socket(socket), _nickname(""), _username(""), _realname(""), _isPassed(false), _isRegistered(false)  {}

Client::~Client() {}

Client::Client(const Client &copy) {
	*this = copy;
}

Client &Client::operator=(const Client &copy) {

	if (this != &copy) {
		_socket = copy._socket;
		_nickname = copy._nickname;
		_username = copy._username;
		_realname = copy._realname;
	}

	return (*this);
}

int Client::getSocket() const { return (_socket); }

std::string Client::getNickname() const { return (_nickname); }

std::string Client::getUsername() const { return (_username); }

std::string Client::getRealname() const { return (_realname); }

std::string Client::getIpAddr() const { return (_ipAddr); }

std::string Client::getClientInfo() const {

	return (":" + _nickname + "!" + _username + "@" + _ipAddr);
	
}


void Client::joinChannel(std::string chanName, Channel *channel) { _channels[chanName] = channel; }

void Client::partChannel(std::string chanName) {

	std::map<std::string, Channel *>::iterator it = _channels.find(chanName);

	if (it != _channels.end())
		_channels.erase(it);

}

std::map<std::string, Channel *> Client::getChannels() const { return (_channels); }

std::deque<std::string> Client::getInvited() const { return (_invited); }

std::string Client::getBuffer() const { return (_buffer); }

bool Client::isPassed() const { return (_isPassed); }


void Client::getInvitaion(std::string chanName) { _invited.push_back(chanName); }

void Client::removeInvitaion(std::string chanName) {

	_invited.erase(std::remove(_invited.begin(), _invited.end(), chanName), _invited.end());

}


void Client::setSocket(int socket) { _socket = socket; }

void Client::setNickname(std::string nickname) { _nickname = nickname; }

void Client::setUsername(std::string username) { _username = username; }

void Client::setRealname(std::string realname) { _realname = realname; }

void Client::setIpAddr() {

	struct sockaddr_in addr;
    socklen_t addrSize = sizeof(struct sockaddr_in);

	getsockname(_socket, (sockaddr *)&addr, &addrSize);

	_ipAddr = std::string(inet_ntoa(addr.sin_addr));
}

void Client::setBuffer(std::string string) { _buffer = string; }

void Client::setPassed() { _isPassed = true; }

void Client::setRegistered() { _isRegistered = true; }


void Client::clearBuffer() { _buffer.clear(); };

void Client::addToBuffer(std::string str) { _buffer += str; };

bool Client::isBufferEndNl() {

	if (_buffer.size() == 0)
		return (false);

	return (_buffer.at(_buffer.size() - 1) == '\n');
}

bool Client::isRegistered() {
	return (_isRegistered);
}
