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
		auto curPlayer = iter->second;
		if (curPlayer.m_fdSetIdx == clntfdIdx)
		{
			playerPtr = &curPlayer;
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

void ServerManager::sendWhisper()
{

}

void ServerManager::showRoomInfo()
{

}

void ServerManager::showRoomList()
{

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
		std::cout << player.m_ip << " ::: " << player.m_port << '\n';
		msg += playerInfo;
	}
	msg += "---------------------------------------------------------------\n\r";

	NetworkManager::getInstance().sendMsg(clntfd, msg);
}

void ServerManager::joinRoom()
{

}

int ServerManager::getChatRoomNum(u_int clntfdIdx)
{
	for (auto iter = playerList.begin(); iter != playerList.end(); iter++)
	{
		auto player = iter->second;
		if (player.m_fdSetIdx == clntfdIdx)
		{
			return player.m_roomNum;
		}
	}
	// donghyun : ��ã�������� ������ ����.
	return -2;
}
void ServerManager::broadCastInRoom(u_int clntfdIdx, int roomNum, std::string& msg)
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

	for (size_t i = 0; i < roomPartSize; i++)
	{
		// donghyun : �ڱ� �ڽŵ� ��ε�ĳ������
		NetworkManager::getInstance().sendMsg(room.roomPartInfo[i].first.m_fdSetIdx, broadMsg);
	}
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


