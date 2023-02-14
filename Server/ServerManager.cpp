#include "ServerManager.h"
#include "NetworkManager.h"
#include <iostream>
#include <format>

ServerManager::ServerManager()
{
}

ServerManager& ServerManager::getInstance()
{
	static ServerManager instance;
	return instance;
}

void ServerManager::init()
{

}


void ServerManager::login(Player& player)
{
	std::cout << "before add : "<<player.m_ip << '\n';
	if (addPlayer(player) == -1)
	{
		NetworkManager::getInstance().sendMsg(player.m_fdSetIdx, "해당 이름은 사용할 수 없습니다.\n\r");
	}
	else
	{
		NetworkManager::getInstance().sendMsg(player.m_fdSetIdx, std::format("로그인 되었습니다. ({})\r\n", player.m_name));
	}
}

// donghyun : 도움말 호출
void ServerManager::showHelp(const int clntfd)
{
	std::string msg = "";
	msg.reserve(100);
	msg += "-------------------------------------------------------------- -\n\r";
	msg += "H                         명령어 안내\n\r";
	msg += "US                        이용자 목록 보기\n\r";
	msg += "LT                        대화방 목록 보기\n\r";
	msg += "ST[방번호]                대화방 정보 보기\n\r";
	msg += "PF[상대방ID]              이용자 정보 보기\n\r";
	msg += "TO[상대방ID][메시지]      쪽지 보내기\n\r";
	msg += "O[최대인원][방제목]       대화방 만들기\n\r";
	msg += "J[방번호]                 대화방 참여하기\n\r";
	msg += "X                         끝내기\n\r";
	msg += "-------------------------------------------------------------- -\n\r";

	NetworkManager::getInstance().sendMsg(clntfd, msg);
}


void ServerManager::createRoom()
{

}

void ServerManager::deleteRoom()
{

}

void ServerManager::sendWhisper()
{

}

void ServerManager::showRoomInfo()
{

}

void ServerManager::showRoomList()
{

}

void ServerManager::showPlayerList(const int clntfd)
{
	std::string msg = "";
	msg.reserve(100);

	msg += "------------------------- 이용자 목록 -------------------------\n\r";
	for(auto iter = playerList.begin(); iter != playerList.end(); iter++)
	{
		auto player = iter->second;
		//이용자: aaa              접속지 : 127.0.0.1 : 63695
		std::string playerInfo = std::format("이용자: {}              접속지 : {} : {}\n\r", player.m_name, player.m_ip, player.m_port);
		std::cout << player.m_ip << " ::: " << player.m_port << '\n';
		msg += playerInfo;
	}
	msg += "---------------------------------------------------------------\n\r";

	NetworkManager::getInstance().sendMsg(clntfd, msg);
}

void ServerManager::joinRoom()
{

}

// donghyun : 첫 클라 소켓 연결 요청 시에 사용됨
int ServerManager::addPlayer(Player& player)
{
	if (playerList.find(player.m_name) != playerList.end())
	{
		return -1;
	}
	else
	{
		std::cout << "ip : "<< player.m_ip << '\n';
		playerList[player.m_name] = player;
		playerList.insert(std::make_pair(player.m_name, player));
		std::cout << "insert!" << '\n';
		std::cout << "ip : " << playerList[player.m_name].m_ip << '\n';
		for (auto iter = playerList.begin(); iter != playerList.end(); iter++)
		{
			auto player = iter->second;
			//이용자: aaa              접속지 : 127.0.0.1 : 63695
			std::cout << player.m_ip << " ::: " << player.m_port << '\n';
		}
		//playerList[player.m_name] = player;
		return 1;
	}
}

const unsigned int ServerManager::findPlayerFd(const std::string playerName)
{
	return playerList[playerName].m_fdSetIdx;
}


