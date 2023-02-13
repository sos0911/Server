#pragma once
#include <string>
#include <WinSock2.h>

typedef struct Player
{
public:
	SOCKADDR_IN m_sockInfo;
	std::string m_name;
	Player(SOCKADDR_IN sockInfo, std::string name);
	Player();
	Player(const Player& otherPlayer) { this->m_name = otherPlayer.m_name; this->m_sockInfo = otherPlayer.m_sockInfo; };
}Player;