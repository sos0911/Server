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

	//std::vector<std::pair<Player&, std::string>> roomPartInfo;

	int maxPartCnt, curPartCnt, roomNum;

	// donghyun(0213) : 해당 방 안에 있는 글쓴이 제외 모든 사람에게 채팅 방송
	Room();
	Room(std::string in_roomName, int in_maxPartCnt, Player& in_player);
	// donghyun : 복사생성자는 디폴트로 둔다 (map insert 시 사용됨)
}Room;