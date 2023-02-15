#pragma once
#include <string>
#include <vector>
#include <WinSock2.h>

#define BUF_SIZE 1024
#define IP_SIZE 20

typedef struct Player
{
	std::string m_ip = "";
	u_short m_port;
	SOCKET m_fd;
	std::string m_name;
	//char m_buf[BUF_SIZE] = { 0 };
	// donghyun : -1�̸� ���� ���� �ʰ�, �� ���� ���� �ش� �� ��ȣ
	int m_roomNum = -1;

	//donghyun : �� ó�� Ŭ�� ���� ���� ��û �� �̸��� ���� �� ����ϴ� ������
	Player(char ip[], u_short port, unsigned long fd, std::string name);
	Player(SOCKET clntFd);
	Player();
	Player(const Player& player);
}Player;