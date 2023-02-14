#include "Room.h"
#include <chrono>
#include <ctime>
#include "ServerManager.h"

Room::Room(std::string in_roomName, int in_maxPartCnt, Player& in_player)
{
	// donghyun : ���� �ð� ĳ��
	auto now = std::chrono::system_clock::now();
	std::time_t current_time = std::chrono::system_clock::to_time_t(now);
	char time_str[9];
	std::strftime(time_str, sizeof(time_str), "%T", std::localtime(&current_time));

	openTime = time_str;
	roomName = in_roomName;
	
	roomPartInfo.push_back({ in_player, time_str });
	
	maxPartCnt = in_maxPartCnt;
	// donghyun : �ڱ� �ڽ�
	curPartCnt = 1;
	roomNum = ServerManager::getInstance().getLastRoomNum();
}

// donghyun : ���� ���� ���� ����ؾ���!
void Room::broadcastChat(Player& chattedPlayer)
{

}
