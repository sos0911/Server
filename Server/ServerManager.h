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

	// donghyun : key�� name�̴�.
	std::map<std::string, Player> playerList;
	std::map<std::string, Room> roomList;

public:

	// donghyun : �̱��� ����
	static ServerManager& getInstance();

	void init();
	void login(Player& player);
	void showHelp(const int clntfd);
	void createRoom();
	// donghyun (0213) : �̰� ���ǿ� ���� ���� �� �����ϰ� �ؾ� �ҵ�
	void deleteRoom();
	void sendWhisper();
	void showRoomInfo();
	void showRoomList();
	void showPlayerList(const int clntfd);
	void joinRoom();

	int addPlayer(Player& player);
	const unsigned int findPlayerFd(const std::string playerName);
};