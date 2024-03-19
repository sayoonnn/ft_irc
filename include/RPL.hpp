
#pragma once

// Welcome
#define RPL_WELCOME(nickname)					(":ircserv 001 " + (nickname) + " :Welcome to the ircserv, " + (nickname) + "\n")
#define RPL_YOURHOST(nickname)					(":ircserv 002 " + (nickname) + " :Your host is ircserv, running version 1.0\n")
#define RPL_CREATED(nickname, datetime)			(":ircserv 003 " + (nickname) + " :This server was created " + (datetime)  + "\n")
#define RPL_MYINFO(nickname)					(":ircserv 004 " + (nickname) + " ircserv 1.0 aw abeiIklmnoOt\n")

// errors
#define ERR_UNKNOWNCOMMAND(nickname, command)	(":ircserv 421 " + (nickname) + " " + (command) + "")
#define ERR_NEEDMOREPARAMS(nickname, command)	(":ircserv 461 " + (nickname) + " " + (command) + " :Not enough parameters")
#define ERR_ALREADYREGISTERED(nickname)			(":ircserv 462 " + (nickname) + " :You may not reregister")
#define ERR_PASSWDMISMATCH(nickname)			(":ircserv 464 " + (nickname) + " :Password incorrect")
