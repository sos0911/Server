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


void ServerManager::createRoom(const int clntfdIdx, std::string maxCntStr, std::string roomName)
{
	Player* playerPtr = nullptr;
	for (auto iter = playerList.begin(); iter != playerList.end(); iter++)
	{
		auto curPlayer = iter->second;
		if (curPlayer.m_fdSetIdx == clntfdIdx)
		{
			playerPtr = &curPlayer;
			break;
		}
	}
	Room room(roomName, std::stoi(maxCntStr), *playerPtr);
	roomList[room.roomNum] = room;
	// donghyun : 방장에게도 속한 방이 있다고 표시해주기
	playerPtr->m_roomNum = room.roomNum;
	
	std::string msg = "";
	msg += "** 대화방이 개설되었습니다.\n\r";
	msg += std::format("**{}님이 들어오셨습니다. (현재인원 {} / {})\n\r", playerPtr->m_name, room.curPartCnt, room.maxPartCnt);
	NetworkManager::getInstance().sendMsg(clntfdIdx, msg);
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

int ServerManager::getChatRoomNum(u_int clntfdIdx)
{
	for (auto iter = playerList.begin(); iter != playerList.end(); iter++)
	{
		auto player = iter->second;
		if (player.m_fdSetIdx == clntfdIdx)
		{
			return player.m_roomNum;
		}
	}
	// donghyun : 못찾았을때는 문제가 있음.
	return -2;
}
void ServerManager::broadCastInRoom(u_int clntfdIdx, int roomNum, std::string& msg)
{
	Room& room = roomList[roomNum];
	size_t roomPartSize = room.roomPartInfo.size();

	std::string broadMsg = "";
	for (auto iter = playerList.begin(); iter != playerList.end(); iter++)
	{
		auto player = iter->second;
		if (player.m_fdSetIdx == clntfdIdx)
		{
			broadMsg += player.m_name + " > ";
			break;
		}
	}
	broadMsg += msg + "\n\r";

	for (size_t i = 0; i < roomPartSize; i++)
	{
		// donghyun : 자기 자신도 브로드캐스팅함
		NetworkManager::getInstance().sendMsg(room.roomPartInfo[i].first.m_fdSetIdx, broadMsg);
	}
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
		playerList.insert(std::make_pair(player.m_name, player));
		//playerList[player.m_name] = player;
		return 1;
	}
}

const unsigned int ServerManager::findPlayerFd(const std::string playerName)
{
	return playerList[playerName].m_fdSetIdx;
}


