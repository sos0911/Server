#pragma once
#include "Player.h"
#include "Room.h"
#include <map>
#include <WinSock2.h>
#include "NetworkManager.h"
#include "Singleton.h"

class ServerManager : public Singleton<ServerManager>
{
private:
	friend class Singleton;
	ServerManager();
	ServerManager(const ServerManager& ref) = delete;
	ServerManager& operator=(const ServerManager& ref) = delete;
	virtual ~ServerManager() {}

	// donghyun : key는 name이다.
	std::map<SOCKET, Player> playerList;
	// donghyun : key는 방 번호이다.
	std::map<int, Room> roomList;
	int lastRoomNum = 1;

public:

	//// donghyun : 싱글톤 구현
	//static ServerManager& getInstance();

	//ServerManager();
	//virtual ~ServerManager() {}

	void login(SOCKET clntfd, std::vector<std::string>& splitStrList);
	void showHelp(const SOCKET clntfd);
	void showChatHelp(const SOCKET clntfd);
	void createRoom(const SOCKET clntfd, std::string& maxCntStr, std::string& roomName);
	void sendWhisper(std::vector<std::string>& splitStrList, const SOCKET clntfd);
	void showRoomInfo(int roomNum, const SOCKET clntfd);
	void showRoomList(const SOCKET clntfd);
	void showPlayerInfo(std::string& playerName, const SOCKET clntfd);
	void showPlayerList(const SOCKET clntfd);

	void joinRoom(const int roomNum, const SOCKET clntfd);

	int getLastRoomNum() { return ++lastRoomNum; }

	int getChatRoomNum(SOCKET clntfd);
	void broadCastChatInRoom(SOCKET clntfd, int roomNum, std::string& msg);
	void broadCastInRoom(int roomNum, std::string& msg);
	void quitPlayer(const SOCKET clntfd);
	void quitRoom(const int roomNum, Player* playerPtr);
	bool addPlayer(Player& player);
	Player* findPlayerUsingfd(const SOCKET clntfd);
	Player* findPlayerUsingName(const std::string& playerName);


	std::string getCurTime();
};