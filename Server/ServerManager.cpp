#include "ServerManager.h"
#include "NetworkManager.h"
#include <iostream>
#include <format>

ServerManager::ServerManager()
{
}

void ServerManager::login(SOCKET clntfd, std::vector<std::string>& splitStrList)
{
	// donghyun : �̸� ������
	int vecSize = static_cast<int>(splitStrList.size());
	std::string playerName = "";
	for (int i = 1; i < vecSize - 1; i++)
	{
		playerName += splitStrList[i] + " ";
	}
	playerName += splitStrList[vecSize - 1];

	// donghyun : �̸� �ߺ� �˻�
	if (nullptr == findPlayerUsingName(playerName))
	{
		Player* playerPtr = findPlayerUsingfd(clntfd);
		if (playerPtr)
		{
			playerPtr->m_name = playerName;
			NetworkManager::getInstance().sendMsg(clntfd,
				std::format("** �α��� �Ͽ����ϴ�. {}\n\r", playerPtr->m_name));
			showHelp(clntfd);
		}
	}
	else
	{
		NetworkManager::getInstance().sendMsg(clntfd,
			"�ش� �̸��� ����� �� �����ϴ�.\n\r");
	}
}

// donghyun : ���� ȣ��
void ServerManager::showHelp(const SOCKET clntfd)
{
	std::string msg = "";
	msg.reserve(100);
	msg = "-------------------------------------------------------------- -\n\r\
H                         ��ɾ� �ȳ�\n\r\
US                        �̿��� ��� ����\n\r\
LT                        ��ȭ�� ��� ����\n\r\
ST [���ȣ]               ��ȭ�� ���� ����\n\r\
PF [����ID]             �̿��� ���� ����\n\r\
TO [����ID] [�޽���]    ���� ������\n\r\
O  [�ִ��ο�] [������]    ��ȭ�� �����\n\r\
J  [���ȣ]               ��ȭ�� �����ϱ�\n\r\
X                         ������\n\r\
-------------------------------------------------------------- -\n\r";

	NetworkManager::getInstance().sendMsg(clntfd, msg);
}

void ServerManager::showChatHelp(const SOCKET clntfd)
{
	std::string msg = "";
	msg.reserve(100);
	msg = "-------------------------------------------------------------- -\n\r\
/H                         ��ɾ� �ȳ�\n\r\
/US                        �̿��� ��� ����\n\r\
/LT                        ��ȭ�� ��� ����\n\r\
/ST [���ȣ]               ��ȭ�� ���� ����\n\r\
/PF [����ID]             �̿��� ���� ����\n\r\
/TO [����ID] [�޽���]    ���� ������\n\r\
/Q                         ��ȭ�� ������\n\r\
/X                         ������\n\r\
-------------------------------------------------------------- -\n\r";

	NetworkManager::getInstance().sendMsg(clntfd, msg);
}

void ServerManager::createRoom(const SOCKET clntfd, std::string& maxCntStr, std::string& roomName)
{
	Player* playerPtr = findPlayerUsingfd(clntfd);
	if (playerPtr)
	{
		Room room(roomName, std::stoi(maxCntStr), *playerPtr);
		roomList[room.roomNum] = room;
		// donghyun : ���忡�Ե� ���� ���� �ִٰ� ǥ�����ֱ�
		playerPtr->m_roomNum = room.roomNum;

		std::string msg = std::format("** ��ȭ���� �����Ǿ����ϴ�.\n\r** {}���� �����̽��ϴ�. (�����ο� {} / {})\n\r", playerPtr->m_name, room.curPartCnt, room.maxPartCnt);
		NetworkManager::getInstance().sendMsg(clntfd, msg);
	}
}

void ServerManager::sendWhisper(std::vector<std::string>& splitStrList, const SOCKET clntfd)
{
	std::string msg = "";
	msg.reserve(100);

	Player* sendPlayerPtr = findPlayerUsingName(splitStrList[1]);
	if (sendPlayerPtr)
	{
		if (clntfd == sendPlayerPtr->m_fd)
		{
			NetworkManager::getInstance().sendMsg(clntfd, "** �ڱ� �ڽſ��Դ� ���� �� �����ϴ�.\n\r");
			return;
		}

		Player& player = playerList[clntfd];
		msg = std::format("# {}���� ���� ==> ", player.m_name);
		size_t splitStrSize = splitStrList.size();
		for (size_t i = 2; i < splitStrSize; ++i)
		{
			msg += splitStrList[i] + " ";
		}
		msg += "\n\r";

		NetworkManager::getInstance().sendMsg(sendPlayerPtr->m_fd, msg);
		NetworkManager::getInstance().sendMsg(clntfd, "** ������ ���½��ϴ�.\n\r");
	}
	else
	{
		msg = std::format("** {}���� ã�� �� �����ϴ�.\n\r", splitStrList[1]);
		NetworkManager::getInstance().sendMsg(clntfd, msg);
	}
}

void ServerManager::showRoomInfo(int roomNum, const SOCKET clntfd)
{
	std::string msg = "";
	msg.reserve(100);

	if (roomList.find(roomNum) == roomList.end())
	{
		msg = "** �������� �ʴ� ��ȭ���Դϴ�.\n\r";
	}
	else
	{
		Room& room = roomList[roomNum];
		msg = std::format("------------------------- ��ȭ�� ���� -------------------------\n\r\
[  {}] ( {}/ {}) {}\n\r\
   �����ð�:  {}\n\r", room.roomNum, room.curPartCnt, room.maxPartCnt, room.roomName, room.openTime);

		for (auto iter = room.roomPartInfo.begin(); iter != room.roomPartInfo.end(); ++iter)
		{
			// donghyun : �ڱ� �ڽŵ� ��ε�ĳ������
			auto& playerInfo = iter->second;
			msg += std::format("   ������: {}              �����ð�:  {}\n\r",
				playerInfo.first->m_name, playerInfo.second);
		}

		msg += "---------------------------------------------------------------\n\r";
	}

	NetworkManager::getInstance().sendMsg(clntfd, msg);
}

void ServerManager::showRoomList(const SOCKET clntfd)
{
	std::string msg = "";
	msg.reserve(100);
	
	msg += "------------------------- ��ȭ�� ��� -------------------------\n\r";
	for (auto iter = roomList.begin(); iter != roomList.end(); ++iter)
	{
		Room& room = iter->second;
		std::string roomInfo = std::format(
			"[  {}] ( {}/ {}) {}\n\r", room.roomNum, room.curPartCnt, room.maxPartCnt, room.roomName);
		msg += roomInfo;
	}
	msg += "---------------------------------------------------------------\n\r";

	NetworkManager::getInstance().sendMsg(clntfd, msg);
}

void ServerManager::showPlayerInfo(std::string& playerName, const SOCKET clntfd)
{
	
	std::string msg = "";
	msg.reserve(100);

	Player* playerPtr = findPlayerUsingName(playerName);
	if(nullptr == playerPtr)
	{
		msg = std::format("** {}���� ã�� �� �����ϴ�.\n\r", playerName);
	}
	else
	{
		Player& player = *playerPtr;
		if (player.m_roomNum > 0)
		{
			msg = std::format("** {}���� ���� {}�� ä�ù濡 �ֽ��ϴ�.\n\r", player.m_name, player.m_roomNum);
		}
		else
		{
			msg = std::format("** {}���� ���� ���ǿ� �ֽ��ϴ�.\n\r", player.m_name);
		}
		msg += std::format("** ������: {} : {}\n\r", player.m_ip, player.m_port);
	}

	NetworkManager::getInstance().sendMsg(clntfd, msg);
}

void ServerManager::showPlayerList(const SOCKET clntfd)
{
	std::string msg = "";
	msg.reserve(100);

	msg += "------------------------- �̿��� ��� -------------------------\n\r";
	for(auto iter = playerList.begin(); iter != playerList.end(); ++iter)
	{
		auto player = iter->second;
		/*std::string playerInfo = std::format("�̿���: {}              ������ : {} : {}\n\r", player.m_name, player.m_ip, player.m_port);
		msg += playerInfo;*/
		msg += player.getInfoStr() + "\n\r";
	}
	msg += "---------------------------------------------------------------\n\r";

	NetworkManager::getInstance().sendMsg(clntfd, msg);
}

void ServerManager::joinRoom(const int roomNum, const SOCKET clntfd)
{
	std::string msg = "";
	msg.reserve(100);

	if (roomList.find(roomNum) == roomList.end())
	{
		msg = std::format("** {}�� ���� �������� �ʴ� ��ȭ���Դϴ�.\n\r", roomNum);
		NetworkManager::getInstance().sendMsg(clntfd, msg);
	}
	else
	{
		Player* playerPtr = findPlayerUsingfd(clntfd);
		if (playerPtr)
		{
			Room& room = roomList[roomNum];
			// donghyun : ���� �ִ� �ο����� �������ٸ� �ο� �ʰ��� ���� �Ұ�
			if (room.curPartCnt >= room.maxPartCnt)
			{
				msg = std::format("** �� �ο� �ʰ��� ������ �Ұ����մϴ�. (�����ο� {} / {})\n\r", 
					room.curPartCnt, room.maxPartCnt);
				NetworkManager::getInstance().sendMsg(clntfd, msg);
				return;
			}
			// donghyun : room�� �ڱ� �ڽ� �߰� (���� �ð���)
			room.roomPartInfo[playerPtr->m_name] = { playerPtr, ServerManager::getInstance().getCurTime() };
			++room.curPartCnt;

			playerPtr->m_roomNum = room.roomNum;

			msg = std::format("**{}���� �����̽��ϴ�. (�����ο� {} / {})\n\r", 
				playerPtr->m_name, room.curPartCnt, room.maxPartCnt);
			broadCastInRoom(room.roomNum, msg);
		}
	}
}

int ServerManager::getChatRoomNum(SOCKET clntfd)
{
	Player* playerPtr = findPlayerUsingfd(clntfd);
	if (playerPtr)
	{
		return playerPtr->m_roomNum;
	}
	// donghyun : ��ã�������� ������ ����.
	return -2;
}

void ServerManager::broadCastChatInRoom(SOCKET clntfd, int roomNum, std::string& msg)
{
	Room& room = roomList[roomNum];

	std::string broadMsg = "";
	Player* playerPtr = findPlayerUsingfd(clntfd);
	if (playerPtr)
	{
		broadMsg = std::format("{} > {}\n\r", playerPtr->m_name, msg);

		// donghyun : ���� ������ ä��ġ�� �� ������, �״�� Ŭ��ܿ� ������ �Ѵ�.
		for (auto iter = room.roomPartInfo.begin(); iter != room.roomPartInfo.end(); ++iter)
		{
			Player* iterPtr = iter->second.first;
			if (iterPtr)
			{
				std::string subMsg = iterPtr->m_buf;
				if (iterPtr->m_fd != clntfd)
				{
					if (iterPtr->m_totalStrLen != 0)
					{
						NetworkManager::getInstance().sendMsg(iterPtr->m_fd, "\n\r");
						NetworkManager::getInstance().sendMsg(iterPtr->m_fd, broadMsg);
						NetworkManager::getInstance().sendMsg(iterPtr->m_fd, subMsg.substr(0, playerPtr->m_totalStrLen));
					}
					else
					{
						NetworkManager::getInstance().sendMsg(iterPtr->m_fd, broadMsg);
					}
				}
				else
				{
					NetworkManager::getInstance().sendMsg(iterPtr->m_fd, broadMsg);
				}
			}
		}
	}
}

void ServerManager::broadCastInRoom(int roomNum, std::string& msg)
{
	Room& room = roomList[roomNum];

	for (auto iter = room.roomPartInfo.begin(); iter != room.roomPartInfo.end(); ++iter)
	{
		// donghyun : �ڱ� �ڽŵ� ��ε�ĳ������
		auto& playerInfo = iter->second;
		NetworkManager::getInstance().sendMsg(playerInfo.first->m_fd, msg);
	}
}

void ServerManager::quitPlayer(const SOCKET clntfd)
{
	Player* playerPtr = findPlayerUsingfd(clntfd);
	if (playerPtr)
	{
		if (playerPtr->m_roomNum > 0)
		{
			quitRoom(playerPtr->m_roomNum, playerPtr);
		}
		// donghyun : �÷��̾� set������ ����
		playerList.erase(playerPtr->m_fd);
	}
	NetworkManager::getInstance().closeClient(clntfd);
}

void ServerManager::quitRoom(const int roomNum, Player* playerPtr)
{
	Room& room = roomList[roomNum];
	room.roomPartInfo.erase(playerPtr->m_name);
	room.curPartCnt--;

	playerPtr->m_roomNum = -1;

	std::string msg = "";
	msg.reserve(100);
	msg = std::format("**{}���� �����̽��ϴ�. (�����ο� {} / {})\n\r",
		playerPtr->m_name, room.curPartCnt, room.maxPartCnt);
	broadCastInRoom(room.roomNum, msg);
	NetworkManager::getInstance().sendMsg(playerPtr->m_fd, "**ä�ù濡�� ���Խ��ϴ�.\n\r");

	// donghyun : ���� �ο��� 0�� ���ϸ� �� ����
	if (room.curPartCnt <= 0)
	{
		roomList.erase(room.roomNum);
	}
}

// donghyun : ù Ŭ�� ���� ���� ��û �ÿ� ����
bool ServerManager::addPlayer(Player& player)
{
	return playerList.insert({ player.m_fd, player }).second;
}

// donghyun : ���� ��ã���� ���� nullptr ��ȯ
Player* ServerManager::findPlayerUsingfd(const SOCKET clntfd)
{
	return (playerList.find(clntfd) == playerList.end()? nullptr : &(playerList[clntfd]));
}

// donghyun : ���� ��ã���� ���� nullptr ��ȯ
Player* ServerManager::findPlayerUsingName(const std::string& playerName)
{
	Player* playerPtr = nullptr;
	for (auto iter = playerList.begin(); iter != playerList.end(); ++iter)
	{
		if (iter->second.m_name == playerName)
		{
			playerPtr = &(iter->second);
			break;
		}
	}
	return playerPtr;
}

std::string ServerManager::getCurTime()
{
	// donghyun : HH:MM:SS �������� ���ϴ� �Լ�
	auto now = std::chrono::system_clock::now();
	std::time_t current_time = std::chrono::system_clock::to_time_t(now);
	std::tm local_time{};
	localtime_s(&local_time, &current_time);
	char time_str[9];
	std::strftime(time_str, sizeof(time_str), "%T", &local_time);

	return time_str;
}


