#pragma once
#include <cstring>
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <WinSock2.h>
#include <string>
#include "ServerManager.h"
#include <set>
#include <map>

#define BUF_SIZE 1024
#define _WINSOCK_DEPRECATED_NO_WARNINGS

enum class Command
{
	H,
	J,
	LOGIN,
	LT,
	O,
	PF,
	ST,
	TO,
	US,
	X,
	INITIAL
};

enum class ChatCommand
{
	H,
	Q,
	LOGIN,
	LT,
	PF,
	ST,
	TO,
	US,
	X,
	INITIAL
};

class NetworkManager
{
private:
	SOCKET hServsock, hClntSock;
	SOCKADDR_IN servAdr, clntAdr;
	timeval timeout;
	fd_set reads;

	NetworkManager();
	NetworkManager(const NetworkManager& ref) = delete;
	NetworkManager& operator=(const NetworkManager& ref) = delete;
	~NetworkManager() {}

	std::map<std::string, Command> commandMap =
	{ {"h", Command::H}, {"j", Command::J},
	  {"login", Command::LOGIN}, {"lt", Command::LT},
	  {"o", Command::O}, {"pf", Command::PF},
	  {"st", Command::ST}, {"to", Command::TO},
	  {"us", Command::US}, {"x", Command::X} };

	std::map<std::string, ChatCommand> chatCommandMap =
	{ {"h", ChatCommand::H}, {"q", ChatCommand::Q},
	  {"login", ChatCommand::LOGIN}, {"lt", ChatCommand::LT},
	  {"pf", ChatCommand::PF},
	  {"st", ChatCommand::ST}, {"to", ChatCommand::TO},
	  {"us", ChatCommand::US}, {"x", ChatCommand::X} };

	int adrSize = 0;
	int strLen = 0, fdNum = 0;
	
	int argc = 0;
	char** argv = nullptr;

public:

	// donghyun : ½Ì±ÛÅæ ±¸Çö
	static NetworkManager& getInstance();

	void init(int argc, char* argv[]);
	void execute();
	void ErrorHandling(const char* message);
	void sendMsg(const std::string playerName, const std::string& msg);
	void sendMsg(const SOCKET clntFd, const std::string& msg);
	void closeClient(const SOCKET clntfd);
	void parseAndSelectComm(std::string& parsingMsg, const SOCKET clntfd);
};
