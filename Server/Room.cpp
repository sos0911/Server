#include "Room.h"
#include <chrono>
#include <ctime>
#include "ServerManager.h"

Room::Room(std::string in_roomName, int in_maxPartCnt, Player& in_player)
{
	// donghyun : 현재 시간 캐싱
	auto now = std::chrono::system_clock::now();
	std::time_t current_time = std::chrono::system_clock::to_time_t(now);
	char time_str[9];
	std::strftime(time_str, sizeof(time_str), "%T", std::localtime(&current_time));

	openTime = time_str;
	roomName = in_roomName;
	
	roomPartInfo.push_back({ in_player, time_str });
	
	maxPartCnt = in_maxPartCnt;
	// donghyun : 자기 자신
	curPartCnt = 1;
	roomNum = ServerManager::getInstance().getLastRoomNum();
}

// donghyun : 말한 본인 빼고 방송해야함!
void Room::broadcastChat(Player& chattedPlayer)
{

}
