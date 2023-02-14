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

	std::set<std::string> commandSet = { "h", "j", "login", "lt", "o", "pf", "st", "to", "us", "x"};

	int adrSize;
	int totalStrLen = 0, strLen = 0, fdNum;
	char buf[BUF_SIZE];
	
	int argc;
	char** argv;

public:

	// donghyun : ½Ì±ÛÅæ ±¸Çö
	static NetworkManager& getInstance();

	void init(int argc, char* argv[]);
	void execute();
	void ErrorHandling(const char* message);
	void parse(const int clntfd, const std::string msg);
	void sendMsg(const std::string playerName, const std::string msg);
	void sendMsg(const unsigned int clntFd, const std::string msg);
};

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