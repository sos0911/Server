#pragma once
#include <string>
#include <vector>
#include "Player.h"
#include <map>

typedef struct Room
{
	std::string openTime = "", roomName = "";
	// donghyun : key : 플레이어 이름
	// donghyun : value : <player 정보, 참여 시각(string)>
	std::map<std::string, std::pair<Player*, std::string>> roomPartInfo;

	int maxPartCnt, curPartCnt, roomNum;

	Room();
	Room(std::string& in_roomName, int in_maxPartCnt, Player& in_player);
	// donghyun : 복사생성자는 디폴트로 둔다 (map insert 시 사용됨)
}Room;