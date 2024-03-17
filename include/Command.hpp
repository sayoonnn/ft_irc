
#pragma once

#include "Client.hpp"

class Command {

	private:
		void cmdJOIN(Client &, std::string);
		void cmdWHO(Client &, std::string);
		void cmdMODE(Client &, std::string);
		void cmdINVITE(Client &, std::string);
	public:

		
};
