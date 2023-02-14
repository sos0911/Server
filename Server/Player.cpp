#include "Player.h"
#include <algorithm>
#include <iterator>

Player::Player(char ip[], u_short port, unsigned long fd): m_port(port), m_fd(fd), m_name("")
{
	std::copy(ip, ip + IP_SIZE, m_ip);
}

Player::Player()
{

}
