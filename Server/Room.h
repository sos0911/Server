#pragma once
#include <string>
#include <vector>
#include "Player.h"

typedef struct Room
{
	std::string openTime, roomName;
	// donghyun : <player 정보, 참여 시각(string)>
	std::vector<std::pair<Player&, std::string>> roomPartInfo;
	int maxPartCnt, curPartCnt, roomNum;

	// donghyun(0213) : 해당 방 안에 있는 글쓴이 제외 모든 사람에게 채팅 방송
	Room(std::string in_roomName, int in_maxPartCnt, Player& in_player);
	// donghyun : 복사생성자는 디폴트로 둔다 (map insert 시 사용됨)
	void broadcastChat(Player& chattedPlayer);
}Room;