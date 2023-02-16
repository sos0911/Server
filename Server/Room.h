#pragma once
#include <string>
#include <vector>
#include "Player.h"
#include <map>

typedef struct Room
{
	std::string openTime = "", roomName = "";
	// donghyun : key : �÷��̾� �̸�
	// donghyun : value : <player ����, ���� �ð�(string)>
	std::map<std::string, std::pair<Player*, std::string>> roomPartInfo;

	int maxPartCnt, curPartCnt, roomNum;

	Room();
	Room(std::string& in_roomName, int in_maxPartCnt, Player& in_player);
	// donghyun : ��������ڴ� ����Ʈ�� �д� (map insert �� ����)
}Room;