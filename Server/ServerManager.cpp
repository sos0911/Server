#include "ServerManager.h"
#include "NetworkManager.h"
#include <iostream>

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


void ServerManager::login(const int clntfd, std::string name)
{
	std::string clntfdStr = std::to_string(clntfd);
	if (playerList.find(clntfdStr) == playerList.end())
	{
		std::cout << "�α��� ����! �ش� ���� ��ũ���͸� ���� �÷��̾� ��ü�� �����ϴ�." << '\n';
	}
	else
	{
		playerList[clntfdStr].m_name = name;
		Player player = playerList[clntfdStr];
		// donghyun : Ű�� �ٲ��� �ϹǷ� ���� �� �߰�
		playerList.erase(clntfdStr);
		playerList[player.m_name] = player;
		std::cout << "�α��� �� ó�� �Ϸ�!" << '\n';
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

void ServerManager::joinRoom()
{

}

// donghyun : ù Ŭ�� ���� ���� ��û �ÿ� ����
void ServerManager::addPlayerUsingFd(Player player)
{
	playerList[std::to_string(player.m_fd)] = player;
}

const unsigned long ServerManager::findPlayerFd(const std::string playerName)
{
	return playerList[playerName].m_fd;
}


