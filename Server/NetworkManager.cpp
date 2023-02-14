#include "NetworkManager.h"
#include <sstream>
#include <vector>
#include <iostream>
#include <algorithm>
#include <format>
#include <WS2tcpip.h>


NetworkManager& NetworkManager::getInstance()
{
	static NetworkManager instance;
	return instance;
}

void NetworkManager::init(int in_argc, char* in_argv[])
{
	argc = in_argc;
	argv = in_argv;
}



void NetworkManager::execute()
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
			//puts("time-out!");
			continue;
		}

		for (u_int i = 0; i < reads.fd_count; i++)
		{
			if (FD_ISSET(reads.fd_array[i], &cpyReads))
			{
				// donghyun : 새로 연결 요청하는 클라 소켓의 경우
				if (reads.fd_array[i] == hServsock)
				{
					adrSize = sizeof(clntAdr);
					hClntSock = accept(hServsock, (SOCKADDR*)&clntAdr, &adrSize);

					sendMsg(i, "접속되었습니다.\n\r");

					FD_SET(hClntSock, &reads);
					printf("connected client : %lu\n", static_cast<unsigned long>(hClntSock));
					char clntIP[20] = { 0 };
					printf("client ip : %s, client port : %d\n", inet_ntop(AF_INET, &clntAdr.sin_addr, 
						clntIP, sizeof(clntIP)), ntohs(clntAdr.sin_port));
					/*std::string ipport = clntIP;
					iport += ":";
					ipport += ntohs(clnptAdr.sin_port);
					std::cout << "client ip-port : " << ipport << '\n';*/
					/*int hClntSockFd = static_cast<unsigned long>(hClntSock);
					std::cout << "clntip : " << clntIP << '\n';
					Player player(clntIP, ntohs(clntAdr.sin_port), hClntSockFd, i);
					ServerManager::getInstance().addPlayerUsingFd(player);*/
				}
				// donghyun : 이미 연결된 클라 소켓에게서 데이터를 받는 경우
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
							std::string Msg = serverMsg + echoMsg;

							// 에코용 방송
							sendMsg(i, Msg);

							std::string printMsg = "received message from client : " + echoMsg;
							printf("%s", printMsg.c_str());

							std::string parsingMsg = echoMsg.substr(0, totalStrLen - 2);
							// donghyun : parsing
							std::stringstream sStream(parsingMsg);
							std::vector<std::string> splitStrList;
							std::string splitStr;
							while (sStream >> splitStr)
							{
								splitStrList.push_back(splitStr);
							}

							if (splitStrList.size() == 0 || commandSet.find(splitStrList[0]) == commandSet.end())
							{
								Msg = "정확한 명령어 형식으로 입력해주세요.\r\n";
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

								// donghyun : 명령어 실행
								switch (command)
								{
								case Command::LOGIN:
								{
									// donghyun : ip, 포트 등의 sockaddr_in 정보, 닉네임
									//ServerManager::getInstance().login(clntfd, splitStrList[1]);
									// donghyun : make player struct
									char clntIP[20] = { 0 };
									inet_ntop(AF_INET, &clntAdr.sin_addr,
										clntIP, sizeof(clntIP)), ntohs(clntAdr.sin_port);
									int hClntSockFd = static_cast<unsigned long>(hClntSock);

									

									//Player player(clntIP, ntohs(clntAdr.sin_port), hClntSockFd, i, splitStrList[1]);
									Player player(clntIP, ntohs(clntAdr.sin_port), hClntSockFd, i, splitStrList[1]);
								

									ServerManager::getInstance().login(player);

								}
								case Command::H:
								{
									ServerManager::getInstance().showHelp(i);
									break;
								}
								case Command::US:
								{
									ServerManager::getInstance().showPlayerList(i);
									break;
								}
								}
							}
							totalStrLen = 0;
							break;
						}
					}
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

std::vector<std::string> NetworkManager::parse(const int clntfd, const std::string msg)
{
	std::stringstream sStream(msg);
	std::vector<std::string> splitStrList;
	std::string splitStr, Msg;
	while (sStream >> splitStr)
	{
		splitStrList.push_back(splitStr);
	}

	if (splitStrList.size() == 0 || commandSet.find(splitStrList[0]) == commandSet.end())
	{
		Msg = "정확한 명령어 형식으로 입력해주세요.\r\n";
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
			//ServerManager::getInstance().login(clntfd, splitStrList[1]);
			Msg = std::format("로그인 되었습니다. ({})\r\n", splitStrList[1]);
			break;
		case Command::H:
			ServerManager::getInstance().showHelp(clntfd);
			break;
		case Command::US:
			ServerManager::getInstance().showPlayerList(clntfd);
			break;
		}
	}
	sendMsg(clntfd, Msg);
	return splitStrList;
}

void NetworkManager::sendMsg(const std::string playerName, const std::string msg)
{
	unsigned long readsIdx = ServerManager::getInstance().findPlayerFd(playerName);
	send(reads.fd_array[readsIdx], msg.c_str(), static_cast<int>(msg.size()), 0);
}

void NetworkManager::sendMsg(const unsigned int clntFd, const std::string msg)
{
	send(reads.fd_array[clntFd], msg.c_str(), static_cast<int>(msg.size()), 0);
}

