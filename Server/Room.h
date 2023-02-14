#pragma once
#include <string>
#include <vector>
#include "Player.h"

typedef struct Room
{
	std::string openTime, roomName;
	std::vector<std::pair<Player&, std::string>> roomPartInfo;
	int maxPartCnt, curPartCnt;

	// donghyun(0213) : �ش� �� �ȿ� �ִ� �۾��� ���� ��� ������� ä�� ���
	Room(std::string openTime, std::string roomName, int maxPartCnt, Player& player);
	// donghyun : ��������ڴ� ����Ʈ�� �д� (map insert �� ����)
	void broadcastChat(Player& chattedPlayer);
}Room;