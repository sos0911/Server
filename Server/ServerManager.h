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
	// donghyun (0213) : 이건 대기실에 있을 때만 방 폭파하게 해야 할듯
	void deleteRoom();
	void sendWhisper();
	void showRoomInfo();
	void showRoomList();
	void joinRoom();
	void start();
};