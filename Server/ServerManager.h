#pragma once
#include "Player.h"
#include "Room.h"
#include <map>
#include <WinSock2.h>
#include "NetworkManager.h"

class ServerManager
{
private:
	ServerManager();
	ServerManager(const ServerManager& ref) {}
	ServerManager& operator=(const ServerManager& ref) {}
	~ServerManager() {}

	// donghyun : key는 name이다.
	std::map<std::string, Player> playerList;
	// donghyun : key는 방 번호이다.
	std::map<int, Room> roomList;
	int lastRoomNum = 1;

public:

	// donghyun : 싱글톤 구현
	static ServerManager& getInstance();

	void init();
	void login(Player& player);
	void showHelp(const int clntfd);
	void createRoom(const int clntfdIdx, std::string maxCntStr, std::string roomName);
	// donghyun (0213) : 이건 대기실에 있을 때만 방 폭파하게 해야 할듯
	void deleteRoom();
	void sendWhisper();
	void showRoomInfo();
	void showRoomList();
	void showPlayerList(const int clntfd);
	void joinRoom();
	int getLastRoomNum() { return lastRoomNum++; }
	int getChatRoomNum(u_int clntfdIdx);
	void broadCastInRoom(u_int clntfdIdx, int roomNum, std::string& msg);

	int addPlayer(Player& player);
	const unsigned int findPlayerFd(const std::string playerName);
};