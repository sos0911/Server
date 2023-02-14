#pragma once
#include <string>
#include <vector>
#include <WinSock2.h>

#define BUF_SIZE 1024
#define IP_SIZE 20

typedef struct Player
{
	//char m_ip[IP_SIZE] = { 0 };
	std::string m_ip = "";
	u_short m_port;
	unsigned long m_fd;
	u_int m_fdSetIdx;
	std::string m_name;
	char m_buf[BUF_SIZE] = { 0 };
	// donghyun : -1이면 속해 있지 않고, 그 외의 경우는 해당 방 번호
	int m_roomNum = -1;

	//donghyun : 맨 처음 클라 소켓 연결 요청 시 이름이 없을 때 사용하는 생성자
	Player(char ip[], u_short port, unsigned long fd, u_int fdSetIdx, std::string name);
	Player();
	Player(const Player& player);
}Player;