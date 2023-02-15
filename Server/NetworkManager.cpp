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
				// donghyun : ���� ���� ��û�ϴ� Ŭ�� ������ ���
				if (reads.fd_array[i] == hServsock)
				{
					adrSize = sizeof(clntAdr);
					hClntSock = accept(hServsock, (SOCKADDR*)&clntAdr, &adrSize);

					FD_SET(hClntSock, &reads);

					char clntIP[20] = { 0 };
					inet_ntop(AF_INET, &clntAdr.sin_addr,
						clntIP, sizeof(clntIP)), ntohs(clntAdr.sin_port);
					Player player(clntIP, ntohs(clntAdr.sin_port), hClntSock, "");

					if (!ServerManager::getInstance().addPlayer(player))
					{
						NetworkManager::getInstance().sendMsg(reads.fd_array[i], "�ʱ� ���� ����! �÷��̾� fd ��ħ.\n\r");
					}

					printf("connected client : %lu\n", static_cast<unsigned long>(hClntSock));
					printf("client ip : %s, client port : %d\n", inet_ntop(AF_INET, &clntAdr.sin_addr, 
						clntIP, sizeof(clntIP)), ntohs(clntAdr.sin_port));

					sendMsg(hClntSock, "���ӵǾ����ϴ�. login [�г���] ���·� �α��� �ٶ��ϴ�.\n\r");
				}
				// donghyun : �̹� ����� Ŭ�� ���Ͽ��Լ� �����͸� �޴� ���
				else
				{
					strLen = recv(reads.fd_array[i], buf + totalStrLen, BUF_SIZE - 1, 0);
					totalStrLen += strLen;

					if (strLen == 0)
					{
						ServerManager::getInstance().quitPlayer(i);
						closesocket(cpyReads.fd_array[i]);
					}
					else
					{
						if (buf[totalStrLen - 1] == '\n')
						{
							buf[totalStrLen] = 0;

							std::string echoMsg = buf;
							echoMsg = echoMsg.substr(0, totalStrLen);
							std::string serverMsg = "message from server : ";
							std::string Msg = serverMsg + echoMsg;

							// ���ڿ� ���
							//sendMsg(reads.fd_array[i], Msg);

							std::string printMsg = "received message from client : " + echoMsg;
							printf("%s", printMsg.c_str());

							std::string parsingMsg = echoMsg.substr(0, totalStrLen - 2);

							// donghyun : ä�ù濡 �ִ��� �˻�
							// donghyun : ���� ä�� ����� ��� �Ľ� �ʿ� X
							int roomNum = ServerManager::getInstance().getChatRoomNum(reads.fd_array[i]);
							if (roomNum >= 0)
							{
								ServerManager::getInstance().broadCastChatInRoom(reads.fd_array[i], 
									roomNum, parsingMsg);
							}
							else
							{
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
									sendMsg(reads.fd_array[i], "��Ȯ�� ��ɾ� �������� �Է����ּ���.\n\r");
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

									// donghyun : �α��� ���ϰ� �����Ϸ��� ��츦 ����
									Player* playerPtr = ServerManager::getInstance().findPlayerUsingfd(reads.fd_array[i]);
									if (playerPtr && playerPtr->m_name == "" && command != Command::LOGIN)
									{
										sendMsg(reads.fd_array[i], 
											"** ���� �α����� �������ּ���. login [�г���]\n\r");
									}
									else
									{
										// donghyun : ��ɾ� ����
										switch (command)
										{
										case Command::LOGIN:
										{
											ServerManager::getInstance().login(cpyReads.fd_array[i], splitStrList[1]);
											break;
										}
										case Command::H:
										{
											ServerManager::getInstance().showHelp(cpyReads.fd_array[i]);
											break;
										}
										case Command::US:
										{
											ServerManager::getInstance().showPlayerList(cpyReads.fd_array[i]);
											break;
										}
										case Command::LT:
										{
											ServerManager::getInstance().showRoomList(cpyReads.fd_array[i]);
											break;
										}
										case Command::ST:
										{
											ServerManager::getInstance().showRoomInfo(std::stoi(splitStrList[1]), cpyReads.fd_array[i]);
											break;
										}
										case Command::PF:
										{
											ServerManager::getInstance().showPlayerInfo(splitStrList[1], cpyReads.fd_array[i]);
											break;
										}
										case Command::TO:
										{
											ServerManager::getInstance().sendWhisper(splitStrList, cpyReads.fd_array[i]);
											break;
										}
										case Command::O:
										{
											ServerManager::getInstance().createRoom(cpyReads.fd_array[i], splitStrList[1], splitStrList[2]);
											break;
										}
										case Command::J:
										{
											ServerManager::getInstance().joinRoom(std::stoi(splitStrList[1]), cpyReads.fd_array[i]);
											break;
										}
										case Command::X:
										{
											ServerManager::getInstance().quitPlayer(cpyReads.fd_array[i]);
											break;
										}
										}
									}
								}
								//break;
							}
							totalStrLen = 0;
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

void NetworkManager::sendMsg(const std::string playerName, const std::string msg)
{
	Player* playerPtr = ServerManager::getInstance().findPlayerUsingName(playerName);
	if (playerPtr)
	{
		send(playerPtr->m_fd, msg.c_str(), static_cast<int>(msg.size()), 0);
	}
}

void NetworkManager::closeClient(const SOCKET clntfd)
{
	FD_CLR(clntfd, &reads);
	printf("closed client fd: %lu\n", static_cast<unsigned long>(clntfd));
	closesocket(clntfd);
}

// donghyun : ���� ��ũ���ͷ� send
void NetworkManager::sendMsg(const SOCKET clntFd, const std::string msg)
{
	send(clntFd, msg.c_str(), static_cast<int>(msg.size()), 0);
}

