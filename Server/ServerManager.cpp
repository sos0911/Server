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
	std::cout << "before add : "<<player.m_ip << '\n';
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


void ServerManager::createRoom()
{

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

// donghyun : ù Ŭ�� ���� ���� ��û �ÿ� ����
int ServerManager::addPlayer(Player& player)
{
	if (playerList.find(player.m_name) != playerList.end())
	{
		return -1;
	}
	else
	{
		std::cout << "ip : "<< player.m_ip << '\n';
		playerList[player.m_name] = player;
		playerList.insert(std::make_pair(player.m_name, player));
		std::cout << "insert!" << '\n';
		std::cout << "ip : " << playerList[player.m_name].m_ip << '\n';
		for (auto iter = playerList.begin(); iter != playerList.end(); iter++)
		{
			auto player = iter->second;
			//�̿���: aaa              ������ : 127.0.0.1 : 63695
			std::cout << player.m_ip << " ::: " << player.m_port << '\n';
		}
		//playerList[player.m_name] = player;
		return 1;
	}
}

const unsigned int ServerManager::findPlayerFd(const std::string playerName)
{
	return playerList[playerName].m_fdSetIdx;
}


