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

void ServerManager::init()
{

}


void ServerManager::login(Player& player)
{
	if (addPlayer(player) == -1)
	{
		NetworkManager::getInstance().sendMsg(player.m_fdSetIdx, "�ش� �̸��� ����� �� �����ϴ�.\n\r");
	}
	else
	{
		NetworkManager::getInstance().sendMsg(player.m_fdSetIdx, std::format("�α��� �Ǿ����ϴ�. ({})\r\n", player.m_name));
	}
}

// donghyun : ���� ȣ��
void ServerManager::showHelp(const int clntfd)
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


void ServerManager::createRoom(const int clntfdIdx, std::string maxCntStr, std::string roomName)
{
	Player* playerPtr = nullptr;
	for (auto iter = playerList.begin(); iter != playerList.end(); iter++)
	{
		if (iter->second.m_fdSetIdx == clntfdIdx)
		{
			playerPtr = &(iter->second);
			break;
		}
	}
	Room room(roomName, std::stoi(maxCntStr), *playerPtr);
	roomList[room.roomNum] = room;
	// donghyun : ���忡�Ե� ���� ���� �ִٰ� ǥ�����ֱ�
	playerPtr->m_roomNum = room.roomNum;
	
	std::string msg = "";
	msg += "** ��ȭ���� �����Ǿ����ϴ�.\n\r";
	msg += std::format("**{}���� �����̽��ϴ�. (�����ο� {} / {})\n\r", playerPtr->m_name, room.curPartCnt, room.maxPartCnt);
	NetworkManager::getInstance().sendMsg(clntfdIdx, msg);
}

void ServerManager::deleteRoom()
{

}

void ServerManager::sendWhisper(std::vector<std::string>& splitStrList, const int clntfd)
{
	std::string msg = "";
	msg.reserve(100);

	std::string& playerName = splitStrList[1];

	if (playerList.find(playerName) == playerList.end())
	{
		msg += std::format("** {}���� ã�� �� �����ϴ�.\n\r", playerName);
		NetworkManager::getInstance().sendMsg(clntfd, msg);
	}
	else
	{
		Player& player = playerList[playerName];
		//# aaa���� ���� ==> ���� ��?
		msg += std::format("# {}���� ���� ==> ", player.m_name);
		size_t splitStrSize = splitStrList.size();
		for (size_t i = 2; i < splitStrSize; i++)
		{
			msg += splitStrList[i] + " ";
		}
		msg += "\n\r";
		NetworkManager::getInstance().sendMsg(player.m_fdSetIdx, msg);
		NetworkManager::getInstance().sendMsg(clntfd, "** ������ ���½��ϴ�.\n\r");
	}
}

void ServerManager::showRoomInfo(int roomNum, const int clntfd)
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

void ServerManager::showRoomList(const int clntfd)
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

void ServerManager::showPlayerInfo(std::string playerName, const int clntfd)
{
	
	std::string msg = "";
	msg.reserve(100);

	if (playerList.find(playerName) == playerList.end())
	{
		msg += std::format("** {}���� ã�� �� �����ϴ�.\n\r", playerName);
	}
	else
	{
		Player& player = playerList[playerName];
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

void ServerManager::showPlayerList(const int clntfd)
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

void ServerManager::joinRoom(const int roomNum, const int clntfd)
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
		Player* playerPtr = nullptr;
		for (auto iter = playerList.begin(); iter != playerList.end(); iter++)
		{
			if ((iter->second).m_fdSetIdx == clntfd)
			{
				playerPtr = &(iter->second);
			}
		}
		if (playerPtr)
		{
			Room& room = roomList[roomNum];
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

int ServerManager::getChatRoomNum(u_int clntfdIdx)
{
	for (auto iter = playerList.begin(); iter != playerList.end(); iter++)
	{
		Player& player = iter->second;
		if (player.m_fdSetIdx == clntfdIdx)
		{
			return player.m_roomNum;
		}
	}
	// donghyun : ��ã�������� ������ ����.
	return -2;
}

void ServerManager::broadCastChatInRoom(u_int clntfdIdx, int roomNum, std::string& msg)
{
	Room& room = roomList[roomNum];
	size_t roomPartSize = room.roomPartInfo.size();

	std::string broadMsg = "";
	for (auto iter = playerList.begin(); iter != playerList.end(); iter++)
	{
		auto player = iter->second;
		if (player.m_fdSetIdx == clntfdIdx)
		{
			broadMsg += player.m_name + " > ";
			break;
		}
	}
	broadMsg += msg + "\n\r";

	for (auto iter = room.roomPartInfo.begin(); iter != room.roomPartInfo.end(); iter++)
	{
		// donghyun : �ڱ� �ڽŵ� ��ε�ĳ������
		auto& playerInfo = iter->second;
		NetworkManager::getInstance().sendMsg(playerInfo.first->m_fdSetIdx, broadMsg);
	}
}

void ServerManager::broadCastInRoom(int roomNum, std::string& msg)
{
	Room& room = roomList[roomNum];

	for (auto iter = room.roomPartInfo.begin(); iter != room.roomPartInfo.end(); iter++)
	{
		// donghyun : �ڱ� �ڽŵ� ��ε�ĳ������
		auto& playerInfo = iter->second;
		NetworkManager::getInstance().sendMsg(playerInfo.first->m_fdSetIdx, msg);
	}
}

void ServerManager::quitPlayer(const int clntfdIdx)
{
	Player* playerPtr = nullptr;
	for (auto iter = playerList.begin(); iter != playerList.end(); iter++)
	{
		if (iter->second.m_fdSetIdx == clntfdIdx)
		{
			playerPtr = &(iter->second);
			break;
		}
	}
	if (playerPtr)
	{
		if (playerPtr->m_roomNum > 0)
		{
			quitRoom(playerPtr->m_roomNum, playerPtr);
		}
		// donghyun : �÷��̾� set������ ����
		playerList.erase(playerPtr->m_name);
	}
	// donghyun : �ڿ� �ִ� �ֵ� �ϳ��� ������ �����ֱ�
	for (auto iter = playerList.begin(); iter != playerList.end(); iter++)
	{
		if (iter->second.m_fdSetIdx > clntfdIdx)
		{
			iter->second.m_fdSetIdx--;
		}
	}
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
int ServerManager::addPlayer(Player& player)
{
	if (playerList.find(player.m_name) != playerList.end())
	{
		return -1;
	}
	else
	{
		playerList.insert(std::make_pair(player.m_name, player));
		//playerList[player.m_name] = player;
		return 1;
	}
}

const unsigned int ServerManager::findPlayerFd(const std::string playerName)
{
	return playerList[playerName].m_fdSetIdx;
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


