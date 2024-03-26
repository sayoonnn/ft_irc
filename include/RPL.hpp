
#pragma once

// Welcome
#define RPL_WELCOME(nickname)								(":ircserv 001 " + (nickname) + " :Welcome to the ircserv, " + (nickname) + "\n")
#define RPL_YOURHOST(nickname)								(":ircserv 002 " + (nickname) + " :Your host is ircserv, running version 1.0\n")
#define RPL_CREATED(nickname, datetime)						(":ircserv 003 " + (nickname) + " :This server was created " + (datetime)  + "\n")
#define RPL_MYINFO(nickname)								(":ircserv 004 " + (nickname) + " :ircserv 1.0 aw abeiIklmnoOt\n")

#define RPL_NICK(newnick, oldnick)							(":" + (oldnick) + " NICK " + (newnick) + "\n")
#define RPL_QUIT(nickname, username, reason)				(":" + (nickname) + "!" + (username) + "@localhost QUIT :" + (reason) + "\n")
#define ERR_UNEXPECTQUIT(nickname, username, reason)		(":" + (nickname) + "!" + (username) + "@localhost QUIT :EOF from client" + "\n")
#define ERR_CLOSELINK(nickname, username, reason)			(":ERROR :Closing Link: " + (nickname) + "[" + (username) + "@localhost] (" + (reason) + ")\n")



#define RPL_NOTOPIC(client, channel)						(":ircserv 331 " + (client) + " " + (channel) + " :No topic is set\n")
#define RPL_TOPIC(client, channel, topic)					(":ircserv 332 " + (client) + " " + (channel) + " :" + (topic) + "\n")
#define RPL_TOPICWHOTIME(client, channel, nick, setat)		(":ircserv 333 " + (client) + " " + (channel) + " " + (setat) + "\n")
#define RPL_INVITING(nick, nick2, channel)					(":ircserv 341 " + (nick) + " " + (nick2) + " " + (channel) + "\n")
#define RPL_NAMREPLY(nickname, channel, users)				(":ircserv 353 " + (nickname) + " = " + (channel) + " :" + (users) + "\n")
#define RPL_ENDOFNAMES(nickname, channel)					(":ircserv 366 " + (nickname) + " " + (channel) + " :End of NAMES list.\n")
#define RPL_MOTD(nickname, line)							(":ircserv 372 " + (nickname) + " : " + (line) + "\n")
#define RPL_MOTDSTART(nickname)								(":ircserv 375 " + (nickname) + " :- ircserv Message of the day - \n")
#define RPL_ENDOFMOTD(nickname)								(":ircserv 376 " + (nickname) + " :End of MOTD command.\n")
#define RPL_CHANNELMODEIS(nick, channel, mode)              (":ircserv 324 " + (nick) + " " + (channel) + " " + (mode) + "\n")


// errors
#define ERR_NOSUCHNICK(nickname, nickname2)					(":ircserv 401 " + (nickname) + " " + (nickname2) + " :No such nick/channel\n")
#define ERR_NOSUCHCHANNEL(nickname, channel)				(":ircserv 403 " + (nickname) + " " + (channel) + " :No such channel\n")
#define ERR_CANNOTSENDTOCHAN(nickname, channel)				(":ircserv 403 " + (nickname) + " " + (channel) + " :Cannot send to channel\n")
#define ERR_TOOMANYCHANNELS(nickname, channel)				(":ircserv 405 " + (nickname) + " " + (channel) + " :You have joined too many channels\n")
#define ERR_NOTEXTTOSEND(nickname)							(":ircserv 412 " + (nickname) + " :No text to send\n")
#define ERR_UNKNOWNCOMMAND(nickname, command)				(":ircserv 421 " + (nickname) + " " + (command) + " :Unknown command\n")
#define ERR_ERRONEUSNICKNAME(nickname, newname)				(":ircserv 432 " + (nickname) + " " + (newname) + " :Erroneus nickname\n")
#define ERR_NICKNAMEINUSE(realname, nickname)				(":ircserv 433 " + (realname) + " " + (nickname) + " :Nickname is already in use\n")
#define ERR_USERNOTINCHANNEL(username, nickname, channel)	(":ircserv 441 " + (username) + " " + (nickname) + " " + (channel) + " :They aren't on that channel\n")
#define ERR_NOTONCHANNEL(nickname, channel)	            	(":ircserv 442 " + (nickname) + " " + (channel) + " :You're not on that channel\n")
#define ERR_USERONCHANNEL(username, nickname, channel)		(":ircserv 443 " + (username) + " " + (nickname) + " " + (channel) + " :is already on channel\n")
#define ERR_NOTREGISTERED(realname)							(":ircserv 451 " + (realname) + " :You have not registered\n")
#define ERR_NEEDMOREPARAMS(nickname, command)				(":ircserv 461 " + (nickname) + " " + (command) + " :Not enough parameters\n")
#define ERR_ALREADYREGISTERED(nickname)						(":ircserv 462 " + (nickname) + " :You may not reregister\n")
#define ERR_PASSWDMISMATCH(nickname)						(":ircserv 464 " + (nickname) + " :Password incorrect\n")
#define ERR_CHANNELISFULL(nickname, channel)				(":ircserv 471 " + (nickname) + " " + (channel) + " :Cannot join channel (+l)\n")
#define ERR_BADCHANMASK(channel)		        			(":ircserv 476 " + (channel) + " :Bad Channel Mask\n")
#define ERR_CHANOPRIVSNEEDED(nickname, channel)				(":ircserv 482 " + (nickname) + " " + (channel) + " :You're not channel operator\n")
#define ERR_UNKNOWNMODE(nickname, mode)			            (":ircserv 472 " + (nickname) + " " + (mode) + " :is unknown mode char to me\n")
#define ERR_UMODEUNKNOWNFLAG(nickname)			            (":ircserv 501 " + (nickname) + " :Unknown MODE flag\n")
#define ERR_INVITEONLYCHAN(nickname, channel)				(":ircserv 473 " + (nickname) + " " + (channel) + " :Cannot join channel (+i)\n")
#define ERR_BADCHANNELKEY(nickname, channel)				(":ircserv 475 " + (nickname) + " " + (channel) + " :Cannot join channel (+k)\n")