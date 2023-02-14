#pragma once
#include <string>
#include <vector>
#include "Player.h"

typedef struct Room
{
	std::string openTime, roomName;
	// donghyun : <player ����, ���� �ð�(string)>
	std::vector<std::pair<Player&, std::string>> roomPartInfo;
	int maxPartCnt, curPartCnt, roomNum;

	// donghyun(0213) : �ش� �� �ȿ� �ִ� �۾��� ���� ��� ������� ä�� ���
	Room(std::string in_roomName, int in_maxPartCnt, Player& in_player);
	// donghyun : ��������ڴ� ����Ʈ�� �д� (map insert �� ����)
	void broadcastChat(Player& chattedPlayer);
}Room;