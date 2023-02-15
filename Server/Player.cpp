#include "Player.h"
#include <algorithm>
#include <iterator>
#include <iostream>


Player::Player(char ip[], u_short port, unsigned long fd, std::string name) :
	m_ip(ip), m_port(port), m_fd(fd), m_name(name)
{
	std::cout << "m_ip : " << m_ip << '\n';
	//std::copy(ip, ip + IP_SIZE, m_ip);
}

Player::Player(SOCKET clntFd) : 
	m_ip(""), m_port(0), m_fd(clntFd), m_name(""), m_roomNum(-1){}

Player::Player() :
	m_ip(""), m_port(0), m_fd(0), m_name(""), m_roomNum(-1){}

Player::Player(const Player& player)
{
	this->m_ip = player.m_ip;
	this->m_port = player.m_port;
	this->m_fd = player.m_fd;
	this->m_name = player.m_name;
	this->m_roomNum = player.m_roomNum;
}
