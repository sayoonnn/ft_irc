#include "Command.hpp"

void Command::cmdJOIN(Client &client, std::deque<std::string> params) {
    (void)client;
    // param : #channel{,#channel} [key{,key}]   (key is optional)
        // 인자가 3개 이상이 들어올 지 아닐 지 모르겠음. 일단, 2개까지만 처리하도록 하겠음.
    // 0. check if channel name is valid : (ex: #channel)
    std::string channel_name = params[0];
    std::stringstream ss(channel_name);
    while (std::getline(ss, channel_name, ',')) {
        // std::cout << "channel_name : " << channel_name << std::endl;
        if (channel_name[0] != '#') {
            // send error message
            return ;
        }
    }
    // 1. check if channel exists
    // 2. check if user is already in channel
    // 3. check if channel is full
    // 4. check if channel is invite only
    // 5. check if channel is banned
    // 6. check if channel is moderated
    // 7. check if channel is +i or +t or +k or +o or +l
    // 8. join channel
}

void Command::cmdWHO(Client &client, std::deque<std::string> params) {
    (void)client;
    (void)params;
}

void Command::cmdMODE(Client &client, std::deque<std::string> params) {
    (void)client;
    (void)params;
}

void Command::cmdINVITE(Client &client, std::deque<std::string> params) {
    (void)client;
    (void)params;
}
