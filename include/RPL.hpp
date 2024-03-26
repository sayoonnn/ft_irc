
#pragma once

// Welcome
#define RPL_WELCOME(nickname)					(":ircserv 001 " + (nickname) + " :Welcome to the ircserv, " + (nickname) + "\r\n")
#define RPL_YOURHOST(nickname)					(":ircserv 002 " + (nickname) + " :Your host is ircserv, running version 1.0\r\n")
#define RPL_CREATED(nickname, datetime)			(":ircserv 003 " + (nickname) + " :This server was created " + (datetime)  + "\r\n")
#define RPL_MYINFO(nickname)					(":ircserv 004 " + (nickname) + " :ircserv 1.0 aw abeiIklmnoOt\r\n")

#define RPL_MOTD(nickname, line)				(":ircserv 372 " + (nickname) + " : " + (line) + "\r\n")
#define RPL_MOTDSTART(nickname)					(":ircserv 375 " + (nickname) + " :- ircserv Message of the day - \r\n")
#define RPL_ENDOFMOTD(nickname)					(":ircserv 376 " + (nickname) + " :End of MOTD command.\r\n")
#define RPL_NAMREPLY(nickname, channel, users)	(":ircserv 353 " + (nickname) + " = " + (channel) + " :" + (users) + "\r\n")
#define RPL_ENDOFNAMES(nickname, channel)		(":ircserv 366 " + (nickname) + " " + (channel) + " :End of NAMES list.\r\n")
#define RPL_INVITING(nick, nick2, channel)		(":ircserv 341 " + (nick) + " " + (nick2) + " " + (channel) + "\r\n")
#define RPL_CHANNELMODEIS(nick, channel, mode)	(":ircserv 324 " + (nick) + " " + (channel) + " " + (mode) + "\r\n")


// errors
#define ERR_UNKNOWNCOMMAND(nickname, command)	(":ircserv 421 " + (nickname) + " " + (command) + " :Unknown command\r\n")
#define ERR_ERRONEUSNICKNAME(nickname, newname)	(":ircserv 432 " + (nickname) + " " + (newname) + " :Erroneus nickname\r\n")
#define ERR_NICKNAMEINUSE(realname, nickname)	(":ircserv 433 " + (realname) + " " + (nickname) + " :Nickname is already in use\r\n")
#define ERR_NOTREGISTERED(realname)				(":ircserv 451 " + (realname) + " :You have not registered\r\n")
#define ERR_NEEDMOREPARAMS(nickname, command)	(":ircserv 461 " + (nickname) + " " + (command) + " :Not enough parameters\r\n")
#define ERR_ALREADYREGISTERED(nickname)			(":ircserv 462 " + (nickname) + " :You may not reregister\r\n")
#define ERR_PASSWDMISMATCH(nickname)			(":ircserv 464 " + (nickname) + " :Password incorrect\r\n")
#define ERR_NOSUCHCHANNEL(nickname, channel)	(":ircserv 403 " + (nickname) + " " + (channel) + " :No such channel\r\n")
#define ERR_TOOMANYCHANNELS(nickname, channel)	(":ircserv 405 " + (nickname) + " " + (channel) + " :You have joined too many channels\r\n")
#define ERR_BADCHANMASK(channel)		        (":ircserv 476 " + (channel) + " :Bad Channel Mask\r\n")
#define ERR_CHANNELISFULL(nickname, channel)	(":ircserv 471 " + (nickname) + " " + (channel) + " :Cannot join channel (+l)\r\n")
#define ERR_TOOMANYCHANNELS(nickname, channel)	(":ircserv 405 " + (nickname) + " " + (channel) + " :You have joined too many channels\r\n")
#define ERR_NOTONCHANNEL(nickname, channel)		(":ircserv 442 " + (nickname) + " " + (channel) + " :You're not on that channel\r\n")
#define ERR_CHANOPRIVSNEEDED(nickname, channel)	(":ircserv 482 " + (nickname) + " " + (channel) + " :You're not channel operator\r\n")
#define ERR_NOSUCHNICK(nickname, nickname2)		(":ircserv 401 " + (nickname) + " " + (nickname2) + " :No such nick/channel\r\n")
#define ERR_USERONCHANNEL(nick, nick2, channel)	(":ircserv 443 " + (nick) + " " + (nick2) + " " + (channel) + " :is already on channel\r\n")
#define ERR_UNKNOWNMODE(nickname, mode)			(":ircserv 472 " + (nickname) + " " + (mode) + " :is unknown mode char to me\r\n")
#define ERR_UMODEUNKNOWNFLAG(nickname)			(":ircserv 501 " + (nickname) + " :Unknown MODE flag\r\n")
#define ERR_USERNOTINCHANNEL(n, n2, channel)    (":ircserv 441 " + (n) + " " + (n2) + " " + (channel) + " :They aren't on that channel\r\n")