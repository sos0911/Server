#include "Room.h"
#include <ctime>

Room::Room(std::string in_openTime, std::string in_roomName, int in_maxPartCnt, Player& in_player)
{
	openTime = in_openTime;
	roomName = in_roomName;
	maxPartCnt = in_maxPartCnt;

	// donghyun : 현재 시간 캐싱
	time_t timer = 0;
	struct tm t;
	localtime_s(&t, &timer);
	// TODO
}

// donghyun : 말한 본인 빼고 방송해야함!
void Room::broadcastChat(Player& chattedPlayer)
{

}
