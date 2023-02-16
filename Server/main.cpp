#include <cstring>
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <WinSock2.h>
#include <string>
#include "NetworkManager.h"
#include "ServerManager.h"

#define BUF_SIZE 1024
#define _WINSOCK_DEPRECATED_NO_WARNINGS

int main(int argc, char* argv[])
{
	// donghyun : �� �Ŵ����� �̱������� ���
	NetworkManager& networkManager = NetworkManager::getInstance();
	networkManager.init(argc, argv);

	networkManager.execute();
	return 0;
}