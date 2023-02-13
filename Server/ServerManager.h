#pragma once
#include "Player.h"
#include "Room.h"
#include <map>
#include <WinSock2.h>

class ServerManager
{
private:
	std::map<std::string, Player> playerList;
	std::map<std::string, Room> roomList;
public:
	ServerManager();
	void login(SOCKADDR_IN sockInfo, std::string name);
	void createRoom();
	// donghyun (0213) : �̰� ���ǿ� ���� ���� �� �����ϰ� �ؾ� �ҵ�
	void deleteRoom();
	void sendWhisper();
	void showRoomInfo();
	void showRoomList();
	void joinRoom();
	void start();
};