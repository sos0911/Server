#include "ServerManager.h"
#include "NetworkManager.h"
#include <iostream>
#include <format>

ServerManager::ServerManager()
{
}

ServerManager& ServerManager::getInstance()
{
	static ServerManager instance;
	return instance;
}

void ServerManager::login(SOCKET clntfd, std::string& playerName)
{
	// donghyun : �̸� �ߺ� �˻�
	if (nullptr == findPlayerUsingName(playerName))
	{
		Player* player = findPlayerUsingfd(clntfd);
		if (player)
		{
			player->m_name = playerName;
		}
		NetworkManager::getInstance().sendMsg(clntfd,
			std::format("** �α��� �Ͽ����ϴ�. {}\n\r", player->m_name));
		showHelp(clntfd);
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
	msg += "-------------------------------------------------------------- -\n\r";
	msg += "H                         ��ɾ� �ȳ�\n\r";
	msg += "US                        �̿��� ��� ����\n\r";
	msg += "LT                        ��ȭ�� ��� ����\n\r";
	msg += "ST[���ȣ]                ��ȭ�� ���� ����\n\r";
	msg += "PF[����ID]              �̿��� ���� ����\n\r";
	msg += "TO[����ID][�޽���]      ���� ������\n\r";
	msg += "O[�ִ��ο�][������]       ��ȭ�� �����\n\r";
	msg += "J[���ȣ]                 ��ȭ�� �����ϱ�\n\r";
	msg += "X                         ������\n\r";
	msg += "-------------------------------------------------------------- -\n\r";

	NetworkManager::getInstance().sendMsg(clntfd, msg);
}


void ServerManager::createRoom(const SOCKET clntfd, std::string maxCntStr, std::string roomName)
{
	Player* playerPtr = findPlayerUsingfd(clntfd);
	if (playerPtr)
	{
		Room room(roomName, std::stoi(maxCntStr), *playerPtr);
		roomList[room.roomNum] = room;
		// donghyun : ���忡�Ե� ���� ���� �ִٰ� ǥ�����ֱ�
		playerPtr->m_roomNum = room.roomNum;

		std::string msg = "";
		msg += "** ��ȭ���� �����Ǿ����ϴ�.\n\r";
		msg += std::format("**{}���� �����̽��ϴ�. (�����ο� {} / {})\n\r", playerPtr->m_name, room.curPartCnt, room.maxPartCnt);
		NetworkManager::getInstance().sendMsg(clntfd, msg);
	}
}

void ServerManager::deleteRoom()
{

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
		//# aaa���� ���� ==> ���� ��?
		msg += std::format("# {}���� ���� ==> ", player.m_name);
		size_t splitStrSize = splitStrList.size();
		for (size_t i = 2; i < splitStrSize; i++)
		{
			msg += splitStrList[i] + " ";
		}
		msg += "\n\r";

		NetworkManager::getInstance().sendMsg(sendPlayerPtr->m_fd, msg);
		NetworkManager::getInstance().sendMsg(clntfd, "** ������ ���½��ϴ�.\n\r");
	}
	else
	{
		msg += std::format("** {}���� ã�� �� �����ϴ�.\n\r", splitStrList[1]);
		NetworkManager::getInstance().sendMsg(clntfd, msg);
	}
}

void ServerManager::showRoomInfo(int roomNum, const SOCKET clntfd)
{
	std::string msg = "";
	msg.reserve(100);

	if (roomList.find(roomNum) == roomList.end())
	{
		msg += "** �������� �ʴ� ��ȭ���Դϴ�.\n\r";
	}
	else
	{
		Room& room = roomList[roomNum];
		msg += "------------------------- ��ȭ�� ���� -------------------------\n\r";

		msg += std::format("[  {}] ( {}/ {}) {}\n\r", 
			room.roomNum, room.curPartCnt, room.maxPartCnt, room.roomName);
		//   �����ð�:  11:43:47
		//   ������: aaa              �����ð�:  11:43:47
		msg += std::format("   �����ð�:  {}\n\r", room.openTime);

		for (auto iter = room.roomPartInfo.begin(); iter != room.roomPartInfo.end(); iter++)
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
	for (auto iter = roomList.begin(); iter != roomList.end(); iter++)
	{
		Room& room = iter->second;
		// [  1] ( 1/ 2) test
		std::string roomInfo = std::format(
			"[  {}] ( {}/ {}) {}\n\r", room.roomNum, room.curPartCnt, room.maxPartCnt, room.roomName);
		msg += roomInfo;
	}
	msg += "---------------------------------------------------------------\n\r";

	NetworkManager::getInstance().sendMsg(clntfd, msg);
}

void ServerManager::showPlayerInfo(std::string playerName, const SOCKET clntfd)
{
	
	std::string msg = "";
	msg.reserve(100);

	Player* playerPtr = findPlayerUsingName(playerName);
	if(nullptr == playerPtr)
	{
		msg += std::format("** {}���� ã�� �� �����ϴ�.\n\r", playerName);
	}
	else
	{
		Player& player = *playerPtr;
		//** bbb���� ���� ���ǿ� �ֽ��ϴ�.
		//** ������: 127.0.0.1 : 57302
		if (player.m_roomNum > 0)
		{
			msg += std::format("** {}���� ���� {}�� ä�ù濡 �ֽ��ϴ�.\n\r", player.m_name, player.m_roomNum);
		}
		else
		{
			msg += std::format("** {}���� ���� ���ǿ� �ֽ��ϴ�.\n\r", player.m_name);
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
	for(auto iter = playerList.begin(); iter != playerList.end(); iter++)
	{
		auto player = iter->second;
		//�̿���: aaa              ������ : 127.0.0.1 : 63695
		std::string playerInfo = std::format("�̿���: {}              ������ : {} : {}\n\r", player.m_name, player.m_ip, player.m_port);
		msg += playerInfo;
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
		msg += std::format("** {}�� ���� �������� �ʴ� ��ȭ���Դϴ�.\n\r", roomNum);
		NetworkManager::getInstance().sendMsg(clntfd, msg);
	}
	else
	{
		Player* playerPtr = findPlayerUsingfd(clntfd);
		if (playerPtr)
		{
			Room& room = roomList[roomNum];
			// donghyun : ���� �ִ� �ο����� �������ٸ� �ο� �ʰ��� ���� �Ұ�
			if (room.curPartCnt + 1 > room.maxPartCnt)
			{
				msg += std::format("**�� �ο� �ʰ��� ������ �Ұ����մϴ�. (�����ο� {} / {})\n\r", 
					room.curPartCnt, room.maxPartCnt);
				NetworkManager::getInstance().sendMsg(clntfd, msg);
				return;
			}
			// donghyun : room�� �ڱ� �ڽ� �߰� (���� �ð���)
			room.roomPartInfo[playerPtr->m_name] = { playerPtr, ServerManager::getInstance().getCurTime() };
			room.curPartCnt++;

			playerPtr->m_roomNum = room.roomNum;

			msg += std::format("**{}���� �����̽��ϴ�. (�����ο� {} / {})\n\r", 
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
	size_t roomPartSize = room.roomPartInfo.size();

	std::string broadMsg = "";
	Player* playerPtr = findPlayerUsingfd(clntfd);
	if (playerPtr)
	{
		broadMsg += playerPtr->m_name + " > ";
		broadMsg += msg + "\n\r";

		// donghyun : ���� ������ ä��ġ�� �� ������, �״�� Ŭ��ܿ� ������ �Ѵ�.
		for (auto iter = room.roomPartInfo.begin(); iter != room.roomPartInfo.end(); iter++)
		{
			Player* playerPtr = iter->second.first;
			if (playerPtr)
			{
				std::string subMsg = playerPtr->m_buf;
				if (playerPtr->m_fd != clntfd)
				{
					if (playerPtr->m_totalStrLen != 0)
					{
						NetworkManager::getInstance().sendMsg(playerPtr->m_fd, "\n\r");
						NetworkManager::getInstance().sendMsg(playerPtr->m_fd, broadMsg);
						NetworkManager::getInstance().sendMsg(playerPtr->m_fd, subMsg.substr(0, playerPtr->m_totalStrLen));
					}
					else
					{
						NetworkManager::getInstance().sendMsg(playerPtr->m_fd, broadMsg);
					}
				}
				else
				{
					NetworkManager::getInstance().sendMsg(playerPtr->m_fd, broadMsg);
				}
			}
		}
	}
}

void ServerManager::broadCastInRoom(int roomNum, std::string& msg)
{
	Room& room = roomList[roomNum];

	for (auto iter = room.roomPartInfo.begin(); iter != room.roomPartInfo.end(); iter++)
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
	msg += std::format("**{}���� �����̽��ϴ�. (�����ο� {} / {})\n\r",
		playerPtr->m_name, room.curPartCnt, room.maxPartCnt);
	broadCastInRoom(room.roomNum, msg);
}

// donghyun : ù Ŭ�� ���� ���� ��û �ÿ� ����
bool ServerManager::addPlayer(Player& player)
{
	if (playerList.find(player.m_fd) == playerList.end())
	{
		playerList[player.m_fd] = player;
		return true;
	}
	else
	{
		return false;
	}
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
	for (auto iter = playerList.begin(); iter != playerList.end(); iter++)
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


