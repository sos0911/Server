#include "Player.h"
#include <algorithm>
#include <iterator>
#include <iostream>


Player::Player(char ip[], u_short port, unsigned long fd, u_int fdSetIdx, std::string name) :
	m_ip(ip), m_port(port), m_fd(fd), m_fdSetIdx(fdSetIdx), m_name(name)
{
	std::cout << "m_ip : " << m_ip << '\n';
	//std::copy(ip, ip + IP_SIZE, m_ip);
}

Player::Player()
{

}

Player::Player(const Player& player)
{
	this->m_ip = player.m_ip;
	this->m_port = player.m_port;
	this->m_fd = player.m_fd;
	this->m_fdSetIdx = player.m_fdSetIdx;
	this->m_name = player.m_name;
}
