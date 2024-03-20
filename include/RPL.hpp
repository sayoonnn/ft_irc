
#pragma once

// Welcome
#define RPL_WELCOME(nickname)					(":ircserv 001 " + (nickname) + " :Welcome to the ircserv, " + (nickname) + "\r\n")
#define RPL_YOURHOST(nickname)					(":ircserv 002 " + (nickname) + " :Your host is ircserv, running version 1.0\r\n")
#define RPL_CREATED(nickname, datetime)			(":ircserv 003 " + (nickname) + " :This server was created " + (datetime)  + "\r\n")
#define RPL_MYINFO(nickname)					(":ircserv 004 " + (nickname) + " :ircserv 1.0 aw abeiIklmnoOt\r\n")

#define RPL_MOTD(nickname, line)				(":ircserv 372 " + (nickname) + " : " + (line) + "\r\n")
#define RPL_MOTDSTART(nickname)					(":ircserv 375 " + (nickname) + " :- ircserv Message of the day - \r\n")
#define RPL_ENDOFMOTD(nickname)					(":ircserv 376 " + (nickname) + " : End of MOTD command.\r\n")


// errors
#define ERR_UNKNOWNCOMMAND(nickname, command)	(":ircserv 421 " + (nickname) + " " + (command) + " :Unknown command\r\n")
#define ERR_ERRONEUSNICKNAME(nickname, newname)	(":ircserv 432 " + (nickname) + " " + (newname) + " :Erroneus nickname\r\n")
#define ERR_NICKNAMEINUSE(realname, nickname)	(":ircserv 433 " + (realname) + " " + (nickname) + " :Nickname is already in use\r\n")
#define ERR_NOTREGISTERED(realname)				(":ircserv 451 " + (realname) + " :You have not registered\r\n")
#define ERR_NEEDMOREPARAMS(nickname, command)	(":ircserv 461 " + (nickname) + " " + (command) + " :Not enough parameters\r\n")
#define ERR_ALREADYREGISTERED(nickname)			(":ircserv 462 " + (nickname) + " :You may not reregister\r\n")
#define ERR_PASSWDMISMATCH(nickname)			(":ircserv 464 " + (nickname) + " :Password incorrect\r\n")
