
#include "Command.hpp"

void Command::cmdJOIN(Client &client, std::string params) {
    (void)client;
    // param : JOIN #channel1,#channel2 key1,key2   (key is optional)
    // 1. check if channel exists
    // 2. check if user is already in channel
    // 3. check if channel is full
    // 4. check if channel is invite only
    // 5. check if channel is banned
    // 6. check if channel is moderated
    // 7. check if channel is +i or +t or +k or +o or +l
    // 8. join channel
}

void Command::cmdWHO(Client &client, std::string params) {
    (void)client;
    (void)params;
}

void Command::cmdMODE(Client &client, std::string params) {
    (void)client;
    (void)params;
}

void Command::cmdINVITE(Client &client, std::string params) {
    (void)client;
    (void)params;
}
