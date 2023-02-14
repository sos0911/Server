#include "ServerManager.h"
#include "NetworkManager.h"
#include <iostream>

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


void ServerManager::login(const int clntfd, std::string name)
{
	std::string clntfdStr = std::to_string(clntfd);
	if (playerList.find(clntfdStr) == playerList.end())
	{
		std::cout << "로그인 오류! 해당 파일 디스크럽터를 가진 플레이어 객체가 없습니다." << '\n';
	}
	else
	{
		playerList[clntfdStr].m_name = name;
		Player player = playerList[clntfdStr];
		// donghyun : 키가 바뀌어야 하므로 삭제 후 추가
		playerList.erase(clntfdStr);
		playerList[player.m_name] = player;
		std::cout << "로그인 및 처리 완료!" << '\n';
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

void ServerManager::joinRoom()
{

}

// donghyun : 첫 클라 소켓 연결 요청 시에 사용됨
void ServerManager::addPlayerUsingFd(Player player)
{
	playerList[std::to_string(player.m_fd)] = player;
}

const unsigned long ServerManager::findPlayerFd(const std::string playerName)
{
	return playerList[playerName].m_fd;
}


