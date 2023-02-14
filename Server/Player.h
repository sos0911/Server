#pragma once
#include <string>
#include <vector>
#include <WinSock2.h>

#define BUF_SIZE 1024
#define IP_SIZE 20

typedef struct Player
{
	char m_ip[IP_SIZE] = { 0 };
	u_short m_port;
	unsigned long m_fd;
	std::string m_name;
	char m_buf[BUF_SIZE];

	//donghyun : 맨 처음 클라 소켓 연결 요청 시 이름이 없을 때 사용하는 생성자
	Player(char ip[], u_short port, unsigned long fd);
	Player();
	Player(const Player& otherPlayer) { this->m_name = otherPlayer.m_name;};
}Player;