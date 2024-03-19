
#pragma once

#include "Client.hpp"
# include <sstream>
#include <deque>

class Command {

	private:
		void cmdJOIN(Client &, std::deque<std::string>);
		void cmdWHO(Client &, std::deque<std::string>);
		void cmdMODE(Client &, std::deque<std::string>);
		void cmdINVITE(Client &, std::deque<std::string>);
	public:

		
};
