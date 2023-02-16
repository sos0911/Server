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
	ServerManager(const ServerManager& ref) = delete;
	ServerManager& operator=(const ServerManager& ref) = delete;
	~ServerManager() {}

	// donghyun : key�� name�̴�.
	std::map<SOCKET, Player> playerList;
	// donghyun : key�� �� ��ȣ�̴�.
	std::map<int, Room> roomList;
	int lastRoomNum = 1;

public:

	// donghyun : �̱��� ����
	static ServerManager& getInstance();

	void login(SOCKET clntfd, std::string& playerName);
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