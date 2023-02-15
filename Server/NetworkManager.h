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

class NetworkManager
{
private:
	WSADATA wsaData;
	SOCKET hServsock, hClntSock;
	SOCKADDR_IN servAdr, clntAdr;
	timeval timeout;
	fd_set reads, cpyReads;

	NetworkManager() {}
	NetworkManager(const NetworkManager& ref) {}
	NetworkManager& operator=(const NetworkManager& ref) {}
	~NetworkManager() {}

	std::map<std::string, Command> commandMap =
	{ {"h", Command::H}, {"j", Command::J},
	  {"login", Command::LOGIN}, {"lt", Command::LT},
	  {"o", Command::O}, {"pf", Command::PF},
	  {"st", Command::ST}, {"to", Command::TO},
	  {"us", Command::US}, {"x", Command::X} };

	int adrSize;
	int strLen = 0, fdNum;
	//int totalStrLen = 0;
	//char buf[BUF_SIZE];
	
	int argc;
	char** argv;

public:

	// donghyun : ½Ì±ÛÅæ ±¸Çö
	static NetworkManager& getInstance();

	void init(int argc, char* argv[]);
	void execute();
	void ErrorHandling(const char* message);
	void sendMsg(const std::string playerName, const std::string msg);
	void sendMsg(const SOCKET clntFd, const std::string msg);
	void closeClient(const SOCKET clntfd);
};
