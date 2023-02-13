#include <cstring>
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <WinSock2.h>
#include <string>
#include "NetworkManager.h"

#define BUF_SIZE 1024
#define _WINSOCK_DEPRECATED_NO_WARNINGS

void ErrorHandling(const char* message);

int main(int argc, char* argv[])
{
	NetworkManager NetworkManager(argc, argv);

	return 0;
}

void ErrorHandling(const char* message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}