#include "ServerManager.h"

ServerManager::ServerManager()
{

	start();
}

void ServerManager::login(SOCKADDR_IN sockInfo, std::string name)
{
	Player player(sockInfo, name);
	playerList[name] = player;
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

void ServerManager::start()
{
	
}