
#pragma once

// Welcome
#define RPL_WELCOME(nickname)					(":ircserv 001 " + (nickname) + " :Welcome to the ircserv, " + (nickname) + "\n")
#define RPL_YOURHOST(nickname)					(":ircserv 002 " + (nickname) + " :Your host is ircserv, running version 1.0\n")
#define RPL_CREATED(nickname, datetime)			(":ircserv 003 " + (nickname) + " :This server was created " + (datetime)  + "\n")
#define RPL_MYINFO(nickname)					(":ircserv 004 " + (nickname) + " ircserv 1.0 aw abeiIklmnoOt\n")
