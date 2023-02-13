#include "NetworkManager.h"
#include <sstream>
#include <vector>
#include <iostream>
#include <algorithm>
#include <format>


NetworkManager::NetworkManager(int argc, char* argv[])
{
	serverManager = std::make_unique<ServerManager>();
	execute(argc, argv);
}

void NetworkManager::execute(int argc, char* argv[])
{
	if (argc != 2)
	{
		printf("usage : %s <port>\n", argv[0]);
		exit(1);
	}
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		ErrorHandling("WSAstartup() error");
	}

	hServsock = socket(PF_INET, SOCK_STREAM, 0);
	if (hServsock == INVALID_SOCKET)
	{
		ErrorHandling("socket() error");
	}
	memset(&servAdr, 0, sizeof(servAdr));
	servAdr.sin_family = AF_INET;
	servAdr.sin_addr.s_addr = htonl(INADDR_ANY);
	servAdr.sin_port = htons(static_cast<short>(atoi(argv[1])));

	if (bind(hServsock, (sockaddr*)&servAdr, sizeof(servAdr)) == SOCKET_ERROR)
	{
		ErrorHandling("bind() error");
	}
	if (listen(hServsock, 5) == SOCKET_ERROR)
	{
		ErrorHandling("listen() error");
	}

	FD_ZERO(&reads);
	FD_SET(hServsock, &reads);

	while (1)
	{
		cpyReads = reads;
		timeout.tv_sec = 5;
		timeout.tv_usec = 5000;

		if ((fdNum = select(0, &cpyReads, 0, 0, &timeout)) == SOCKET_ERROR)
		{
			break;
		}
		if (fdNum == 0)
		{
			puts("time-out!");
			continue;
		}

		for (u_int i = 0; i < reads.fd_count; i++)
		{
			if (FD_ISSET(reads.fd_array[i], &cpyReads))
			{
				if (reads.fd_array[i] == hServsock)
				{
					adrSize = sizeof(clntAdr);
					hClntSock = accept(hServsock, (SOCKADDR*)&clntAdr, &adrSize);
					FD_SET(hClntSock, &reads);
					printf("connected client : %lu\n", static_cast<unsigned long>(hClntSock));
				}
				else
				{
					strLen = recv(reads.fd_array[i], buf + totalStrLen, BUF_SIZE - 1, 0);
					totalStrLen += strLen;
					//printf("receive message : %s\n", buf);
					if (strLen == 0)
					{
						FD_CLR(reads.fd_array[i], &reads);
						closesocket(cpyReads.fd_array[i]);
						printf("closed client : %lu\n", static_cast<unsigned long>(cpyReads.fd_array[i]));
					}
					else
					{
						if (buf[totalStrLen - 1] == '\n')
						{
							buf[totalStrLen] = 0;

							std::string serverMsg = "message from server : ";
							std::string echoMsg = buf;
							echoMsg = echoMsg.substr(0, totalStrLen);
							std::string sendMsg = serverMsg + echoMsg;

							std::string printMsg = "received message from client : " + echoMsg;
							printf("%s", printMsg.c_str());

							std::string parsingMsg = echoMsg.substr(0, totalStrLen - 2);
							// donghyun : parsing
							sendMsg = parse(parsingMsg);

							send(reads.fd_array[i], sendMsg.c_str(), static_cast<int>(sendMsg.size()), 0);
							totalStrLen = 0;
							break;
						}
					}
					/*else
					{
						send(reads.fd_array[i], buf, strLen, 0);
					}*/
				}
			}
		}
	}
	closesocket(hServsock);
	WSACleanup();
}

void NetworkManager::ErrorHandling(const char* message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}

std::string NetworkManager::parse(const std::string msg)
{
	std::stringstream sStream(msg);
	std::vector<std::string> splitStrList;
	std::string splitStr, sendMsg;
	while (sStream >> splitStr)
	{
		splitStrList.push_back(splitStr);
	}

	if (splitStrList.size() == 0 || commandSet.find(splitStrList[0]) == commandSet.end())
	{
		sendMsg = "정확한 명령어 형식으로 입력해주세요.\r\n";
	}
	else
	{
		std::transform(splitStrList[0].begin(), splitStrList[0].end(), splitStrList[0].begin(), [](char const& c)
			{
				return std::tolower(c);
			});
		std::string commandStr = splitStrList[0];
		std::cout << "commandstr : " << commandStr << '\n';
		Command command = Command::INITIAL;
		int setIdx = 0;
		for (auto iter = commandSet.begin(); iter != commandSet.end(); iter++)
		{
			std::string iterStr = *iter;
			if (commandStr == *iter)
			{
				command = static_cast<Command>(setIdx);
				break;
			}
			setIdx++;
		}

		switch(command)
		{
		case Command::LOGIN:
			// donghyun : ip, 포트 등의 sockaddr_in 정보, 닉네임
			serverManager->login(clntAdr, splitStrList[1]);
			sendMsg = std::format("로그인 되었습니다. ({})\r\n", splitStrList[1]);
		}
	}
	return sendMsg;
}
