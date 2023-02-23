#include "NetworkManager.h"
#include <sstream>
#include <vector>
#include <iostream>
#include <algorithm>
#include <format>
#include <WS2tcpip.h>

NetworkManager::NetworkManager()
{
	hServsock = 0;
	hClntSock = 0;
	servAdr = { 0 };
	clntAdr = { 0 };
	timeout = { 0 };
	reads = { 0 };
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
	WSADATA wsaData;
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

	fd_set cpyReads;

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
			continue;
		}

		for (u_int i = 0; i < reads.fd_count; ++i)
		{
			if (!FD_ISSET(reads.fd_array[i], &cpyReads))
			{
				continue;
			}
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
					continue;
				}

				printf("connected client : %lu\n", static_cast<unsigned long>(hClntSock));
				printf("client ip : %s, client port : %d\n", inet_ntop(AF_INET, &clntAdr.sin_addr, 
					clntIP, sizeof(clntIP)), ntohs(clntAdr.sin_port));

				sendMsg(hClntSock, "���ӵǾ����ϴ�. login [�г���] ���·� �α��� �ٶ��ϴ�.\n\r");
			}
			// donghyun : �̹� ����� Ŭ�� ���Ͽ��Լ� �����͸� �޴� ���
			else
			{
				Player* playerPtr = ServerManager::getInstance().findPlayerUsingfd(reads.fd_array[i]);
				if (nullptr == playerPtr)
				{
					continue;
				}
				char* buf = playerPtr->m_buf;
				int& totalStrLen = playerPtr->m_totalStrLen;
				strLen = recv(reads.fd_array[i], buf + totalStrLen, BUF_SIZE - 1, 0);
				totalStrLen += strLen;

				if (strLen == 0)
				{
					ServerManager::getInstance().quitPlayer(i);
					closesocket(cpyReads.fd_array[i]);
				}
				else if(buf[totalStrLen - 1] == '\n' || buf[totalStrLen - 1] == '\0')
				{
					buf[totalStrLen] = 0;

					std::string echoMsg = buf;
					echoMsg = echoMsg.substr(0, totalStrLen);

					std::string printMsg =
						std::format("[{}] : {}", playerPtr->m_name, echoMsg);
					printf("%s", printMsg.c_str());

					std::string parsingMsg = echoMsg.substr(0, totalStrLen - 2);

					parseAndSelectComm(parsingMsg, reads.fd_array[i]);

					totalStrLen = 0;
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

void NetworkManager::sendMsg(const std::string playerName, const std::string& msg)
{
	Player* playerPtr = ServerManager::getInstance().findPlayerUsingName(playerName);
	if (playerPtr)
	{
		send(playerPtr->m_fd, msg.c_str(), static_cast<int>(msg.size()), 0);
	}
}

// donghyun : ���� ��ũ���ͷ� send
void NetworkManager::sendMsg(const SOCKET clntFd, const std::string& msg)
{
	send(clntFd, msg.c_str(), static_cast<int>(msg.size()), 0);
}

void NetworkManager::closeClient(const SOCKET clntfd)
{
	FD_CLR(clntfd, &reads);
	printf("closed client fd: %lu\n", static_cast<unsigned long>(clntfd));
	closesocket(clntfd);
}

void NetworkManager::parseAndSelectComm(std::string& parsingMsg, const SOCKET clntfd)
{
	// donghyun : parsing
	Player* playerPtr = ServerManager::getInstance().findPlayerUsingfd(clntfd);
	if (nullptr == playerPtr)
	{
		return;
	}

	// donghyun : ä�ù濡 ������ msg �� '/' ���� �� �Ľ� �ʿ�
	int roomNum = ServerManager::getInstance().getChatRoomNum(clntfd);
	if (roomNum > 0)
	{
		if (parsingMsg[0] == '/')
		{
			parsingMsg = parsingMsg.substr(1);
		}
		else
		{
			// donghyun : ä�� ���
			ServerManager::getInstance().broadCastChatInRoom(clntfd,
				roomNum, parsingMsg);
			return;
		}
	}

	std::stringstream sStream(parsingMsg);
	std::vector<std::string> splitStrList;
	std::string splitStr;
	while (sStream >> splitStr)
	{
		splitStrList.push_back(splitStr);
	}

	std::transform(splitStrList[0].begin(), splitStrList[0].end(), splitStrList[0].begin(),
		[](unsigned char c) { return std::tolower(c, std::locale("en")); });

	// donghyun : ä�ù濡�� ��ɾ� ��� �� 
	if (roomNum > 0)
	{
		if (splitStrList.size() == 0 || chatCommandMap.find(splitStrList[0]) == chatCommandMap.end())
		{
			sendMsg(clntfd, "��Ȯ�� ��ɾ� �������� �Է����ּ���.\n\r");
			return;
		}

		std::string commandStr = splitStrList[0];
		ChatCommand command = chatCommandMap[commandStr];

		// donghyun : �α��� ���ϰ� �����Ϸ��� ��츦 ����
		if (playerPtr && playerPtr->m_name == "" && command != ChatCommand::LOGIN)
		{
			sendMsg(clntfd, "** ���� �α����� �������ּ���. login [�г���]\n\r");
		}
		else
		{
			// donghyun : ��ɾ� ����
			switch (command)
			{
			case ChatCommand::H:
			{
				if (splitStrList.size() != static_cast<u_int>(1))
				{
					sendMsg(clntfd, "��ɾ� ���ڸ� ��Ȯ�� �Է����ּ���.\n\r");
					break;
				}
				ServerManager::getInstance().showChatHelp(clntfd);
				break;
			}
			case ChatCommand::US:
			{
				if (splitStrList.size() != static_cast<u_int>(1))
				{
					sendMsg(clntfd, "��ɾ� ���ڸ� ��Ȯ�� �Է����ּ���.\n\r");
					break;
				}
				ServerManager::getInstance().showPlayerList(clntfd);
				break;
			}
			case ChatCommand::LT:
			{
				if (splitStrList.size() != static_cast<u_int>(1))
				{
					sendMsg(clntfd, "��ɾ� ���ڸ� ��Ȯ�� �Է����ּ���.\n\r");
					break;
				}
				ServerManager::getInstance().showRoomList(clntfd);
				break;
			}
			case ChatCommand::ST:
			{
				if (splitStrList.size() != static_cast<u_int>(2))
				{
					sendMsg(clntfd, "��ɾ� ���ڸ� ��Ȯ�� �Է����ּ���.\n\r");
					break;
				}
				if (0 == atoi(splitStrList[1].c_str()) && splitStrList[1].compare("0") != 0)
				{
					sendMsg(clntfd, "��Ȯ�� ���� Ÿ���� �־� �ּ���.\n\r");
					break;
				}
				ServerManager::getInstance().showRoomInfo(std::stoi(splitStrList[1]), clntfd);
				break;
			}
			case ChatCommand::PF:
			{
				if (splitStrList.size() != static_cast<u_int>(2))
				{
					sendMsg(clntfd, "��ɾ� ���ڸ� ��Ȯ�� �Է����ּ���.\n\r");
					break;
				}
				ServerManager::getInstance().showPlayerInfo(splitStrList[1], clntfd);
				break;
			}
			case ChatCommand::TO:
			{
				ServerManager::getInstance().sendWhisper(splitStrList, clntfd);
				break;
			}
			case ChatCommand::Q:
			{
				if (splitStrList.size() != static_cast<u_int>(1))
				{
					sendMsg(clntfd, "��ɾ� ���ڸ� ��Ȯ�� �Է����ּ���.\n\r");
					break;
				}
				ServerManager::getInstance().quitRoom(roomNum, playerPtr);
				break;
			}
			case ChatCommand::X:
			{
				if (splitStrList.size() != static_cast<u_int>(1))
				{
					sendMsg(clntfd, "��ɾ� ���ڸ� ��Ȯ�� �Է����ּ���.\n\r");
					break;
				}
				ServerManager::getInstance().quitPlayer(clntfd);
				break;
			}
			}
		}
	}
	// donghyun : ���ǿ��� ��ɾ� ��� ��
	else
	{
		if (splitStrList.size() == 0 || commandMap.find(splitStrList[0]) == commandMap.end())
		{
			sendMsg(clntfd, "��Ȯ�� ��ɾ� �������� �Է����ּ���.\n\r");
			return;
		}

		std::string commandStr = splitStrList[0];
		Command command = commandMap[commandStr];

		// donghyun : �α��� ���ϰ� �����Ϸ��� ��츦 ����
		if (playerPtr && playerPtr->m_name == "" && command != Command::LOGIN)
		{
			sendMsg(clntfd, "** ���� �α����� �������ּ���. login [�г���]\n\r");
		}
		else
		{
			// donghyun : ��ɾ� ����
			switch (command)
			{
			case Command::LOGIN:
			{
				/*if (splitStrList.size() != static_cast<u_int>(2))
				{
					sendMsg(clntfd, "��ɾ� ���ڸ� ��Ȯ�� �Է����ּ���.\n\r");
					break;
				}*/
				ServerManager::getInstance().login(clntfd, splitStrList);
				break;
			}
			case Command::H:
			{
				if (splitStrList.size() != static_cast<u_int>(1))
				{
					sendMsg(clntfd, "��ɾ� ���ڸ� ��Ȯ�� �Է����ּ���.\n\r");
					break;
				}
				ServerManager::getInstance().showHelp(clntfd);
				break;
			}
			case Command::US:
			{
				if (splitStrList.size() != static_cast<u_int>(1))
				{
					sendMsg(clntfd, "��ɾ� ���ڸ� ��Ȯ�� �Է����ּ���.\n\r");
					break;
				}
				ServerManager::getInstance().showPlayerList(clntfd);
				break;
			}
			case Command::LT:
			{
				if (splitStrList.size() != static_cast<u_int>(1))
				{
					sendMsg(clntfd, "��ɾ� ���ڸ� ��Ȯ�� �Է����ּ���.\n\r");
					break;
				}
				ServerManager::getInstance().showRoomList(clntfd);
				break;
			}
			case Command::ST:
			{
				if (splitStrList.size() != static_cast<u_int>(2))
				{
					sendMsg(clntfd, "��ɾ� ���ڸ� ��Ȯ�� �Է����ּ���.\n\r");
					break;
				}
				if (0 == atoi(splitStrList[1].c_str()) && splitStrList[1].compare("0") != 0)
				{
					sendMsg(clntfd, "��Ȯ�� ���� Ÿ���� �־� �ּ���.\n\r");
					break;
				}
				ServerManager::getInstance().showRoomInfo(std::stoi(splitStrList[1]), clntfd);
				break;
			}
			case Command::PF:
			{
				if (splitStrList.size() != static_cast<u_int>(2))
				{
					sendMsg(clntfd, "��ɾ� ���ڸ� ��Ȯ�� �Է����ּ���.\n\r");
					break;
				}
				ServerManager::getInstance().showPlayerInfo(splitStrList[1], clntfd);
				break;
			}
			case Command::TO:
			{
				/*if (splitStrList.size() != static_cast<u_int>(3))
				{
					sendMsg(clntfd, "��ɾ� ���ڸ� ��Ȯ�� �Է����ּ���.\n\r");
					break;
				}*/
				ServerManager::getInstance().sendWhisper(splitStrList, clntfd);
				break;
			}
			case Command::O:
			{
				if (splitStrList.size() != static_cast<u_int>(3))
				{
					sendMsg(clntfd, "��ɾ� ���ڸ� ��Ȯ�� �Է����ּ���.\n\r");
					break;
				}
				if (0 == atoi(splitStrList[1].c_str()) && splitStrList[1].compare("0") != 0)
				{
					sendMsg(clntfd, "��Ȯ�� ���� Ÿ���� �־� �ּ���.\n\r");
					break;
				}
				ServerManager::getInstance().createRoom(clntfd, splitStrList[1], splitStrList[2]);
				break;
			}
			case Command::J:
			{
				if (splitStrList.size() != static_cast<u_int>(2))
				{
					sendMsg(clntfd, "��ɾ� ���ڸ� ��Ȯ�� �Է����ּ���.\n\r");
					break;
				}
				if (0 == atoi(splitStrList[1].c_str()) && splitStrList[1].compare("0") != 0)
				{
					sendMsg(clntfd, "��Ȯ�� ���� Ÿ���� �־� �ּ���.\n\r");
					break;
				}
				ServerManager::getInstance().joinRoom(std::stoi(splitStrList[1]), clntfd);
				break;
			}
			case Command::X:
			{
				if (splitStrList.size() != static_cast<u_int>(1))
				{
					sendMsg(clntfd, "��ɾ� ���ڸ� ��Ȯ�� �Է����ּ���.\n\r");
					break;
				}
				ServerManager::getInstance().quitPlayer(clntfd);
				break;
			}
			}
		}
	}
}
