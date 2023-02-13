#pragma once
#include <cstring>
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <WinSock2.h>
#include <string>
#include "ServerManager.h"
#include <set>

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

	std::unique_ptr<ServerManager> serverManager;
	std::set<std::string> commandSet = { "h", "j", "login", "lt", "o", "pf", "st", "to", "us", "x"};

	int adrSize;
	int totalStrLen = 0, strLen = 0, fdNum;
	char buf[BUF_SIZE];

public:
	NetworkManager(int argc, char* argv[]);
	void execute(int argc, char* argv[]);
	void ErrorHandling(const char* message);
	std::string parse(const std::string msg);
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