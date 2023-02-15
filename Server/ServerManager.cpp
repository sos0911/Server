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

void ServerManager::login(SOCKET clntfd, std::string& playerName)
{
	// donghyun : 이름 중복 검사
	if (nullptr == findPlayerUsingName(playerName))
	{
		Player* player = findPlayerUsingfd(clntfd);
		if (player)
		{
			player->m_name = playerName;
		}
		NetworkManager::getInstance().sendMsg(clntfd,
			std::format("** 로그인 하였습니다. {}\n\r", player->m_name));
		showHelp(clntfd);
	}
	else
	{
		NetworkManager::getInstance().sendMsg(clntfd,
			"해당 이름은 사용할 수 없습니다.\n\r");
	}
}

// donghyun : 도움말 호출
void ServerManager::showHelp(const SOCKET clntfd)
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


void ServerManager::createRoom(const SOCKET clntfd, std::string maxCntStr, std::string roomName)
{
	Player* playerPtr = findPlayerUsingfd(clntfd);
	if (playerPtr)
	{
		Room room(roomName, std::stoi(maxCntStr), *playerPtr);
		roomList[room.roomNum] = room;
		// donghyun : 방장에게도 속한 방이 있다고 표시해주기
		playerPtr->m_roomNum = room.roomNum;

		std::string msg = "";
		msg += "** 대화방이 개설되었습니다.\n\r";
		msg += std::format("**{}님이 들어오셨습니다. (현재인원 {} / {})\n\r", playerPtr->m_name, room.curPartCnt, room.maxPartCnt);
		NetworkManager::getInstance().sendMsg(clntfd, msg);
	}
}

void ServerManager::deleteRoom()
{

}

void ServerManager::sendWhisper(std::vector<std::string>& splitStrList, const SOCKET clntfd)
{
	std::string msg = "";
	msg.reserve(100);

	Player* sendPlayerPtr = findPlayerUsingName(splitStrList[1]);
	if (sendPlayerPtr)
	{
		if (clntfd == sendPlayerPtr->m_fd)
		{
			NetworkManager::getInstance().sendMsg(clntfd, "** 자기 자신에게는 보낼 수 없습니다.\n\r");
			return;
		}

		Player& player = playerList[clntfd];
		//# aaa님의 쪽지 ==> 띄어쓰기 됨?
		msg += std::format("# {}님의 쪽지 ==> ", player.m_name);
		size_t splitStrSize = splitStrList.size();
		for (size_t i = 2; i < splitStrSize; i++)
		{
			msg += splitStrList[i] + " ";
		}
		msg += "\n\r";

		NetworkManager::getInstance().sendMsg(sendPlayerPtr->m_fd, msg);
		NetworkManager::getInstance().sendMsg(clntfd, "** 쪽지를 보냈습니다.\n\r");
	}
	else
	{
		msg += std::format("** {}님을 찾을 수 없습니다.\n\r", splitStrList[1]);
		NetworkManager::getInstance().sendMsg(clntfd, msg);
	}
}

void ServerManager::showRoomInfo(int roomNum, const SOCKET clntfd)
{
	std::string msg = "";
	msg.reserve(100);

	if (roomList.find(roomNum) == roomList.end())
	{
		msg += "** 존재하지 않는 대화방입니다.\n\r";
	}
	else
	{
		Room& room = roomList[roomNum];
		msg += "------------------------- 대화방 정보 -------------------------\n\r";

		msg += std::format("[  {}] ( {}/ {}) {}\n\r", 
			room.roomNum, room.curPartCnt, room.maxPartCnt, room.roomName);
		//   개설시간:  11:43:47
		//   참여자: aaa              참여시간:  11:43:47
		msg += std::format("   개설시간:  {}\n\r", room.openTime);

		for (auto iter = room.roomPartInfo.begin(); iter != room.roomPartInfo.end(); iter++)
		{
			// donghyun : 자기 자신도 브로드캐스팅함
			auto& playerInfo = iter->second;
			msg += std::format("   참여자: {}              참여시간:  {}\n\r",
				playerInfo.first->m_name, playerInfo.second);
		}

		msg += "---------------------------------------------------------------\n\r";
	}

	NetworkManager::getInstance().sendMsg(clntfd, msg);
}

void ServerManager::showRoomList(const SOCKET clntfd)
{
	std::string msg = "";
	msg.reserve(100);
	
	msg += "------------------------- 대화방 목록 -------------------------\n\r";
	for (auto iter = roomList.begin(); iter != roomList.end(); iter++)
	{
		Room& room = iter->second;
		// [  1] ( 1/ 2) test
		std::string roomInfo = std::format(
			"[  {}] ( {}/ {}) {}\n\r", room.roomNum, room.curPartCnt, room.maxPartCnt, room.roomName);
		msg += roomInfo;
	}
	msg += "---------------------------------------------------------------\n\r";

	NetworkManager::getInstance().sendMsg(clntfd, msg);
}

void ServerManager::showPlayerInfo(std::string playerName, const SOCKET clntfd)
{
	
	std::string msg = "";
	msg.reserve(100);

	Player* playerPtr = findPlayerUsingName(playerName);
	if(nullptr == playerPtr)
	{
		msg += std::format("** {}님을 찾을 수 없습니다.\n\r", playerName);
	}
	else
	{
		Player& player = *playerPtr;
		//** bbb님은 현재 대기실에 있습니다.
		//** 접속지: 127.0.0.1 : 57302
		if (player.m_roomNum > 0)
		{
			msg += std::format("** {}님은 현재 {}번 채팅방에 있습니다.\n\r", player.m_name, player.m_roomNum);
		}
		else
		{
			msg += std::format("** {}님은 현재 대기실에 있습니다.\n\r", player.m_name);
		}
		msg += std::format("** 접속지: {} : {}\n\r", player.m_ip, player.m_port);
	}

	NetworkManager::getInstance().sendMsg(clntfd, msg);
}

void ServerManager::showPlayerList(const SOCKET clntfd)
{
	std::string msg = "";
	msg.reserve(100);

	msg += "------------------------- 이용자 목록 -------------------------\n\r";
	for(auto iter = playerList.begin(); iter != playerList.end(); iter++)
	{
		auto player = iter->second;
		//이용자: aaa              접속지 : 127.0.0.1 : 63695
		std::string playerInfo = std::format("이용자: {}              접속지 : {} : {}\n\r", player.m_name, player.m_ip, player.m_port);
		msg += playerInfo;
	}
	msg += "---------------------------------------------------------------\n\r";

	NetworkManager::getInstance().sendMsg(clntfd, msg);
}

void ServerManager::joinRoom(const int roomNum, const SOCKET clntfd)
{
	std::string msg = "";
	msg.reserve(100);

	if (roomList.find(roomNum) == roomList.end())
	{
		msg += std::format("** {}번 방은 존재하지 않는 대화방입니다.\n\r", roomNum);
		NetworkManager::getInstance().sendMsg(clntfd, msg);
	}
	else
	{
		Player* playerPtr = findPlayerUsingfd(clntfd);
		if (playerPtr)
		{
			Room& room = roomList[roomNum];
			// donghyun : 만약 최대 인원보다 많아진다면 인원 초과로 입장 불가
			if (room.curPartCnt + 1 > room.maxPartCnt)
			{
				msg += std::format("**방 인원 초과로 입장이 불가능합니다. (현재인원 {} / {})\n\r", 
					room.curPartCnt, room.maxPartCnt);
				NetworkManager::getInstance().sendMsg(clntfd, msg);
				return;
			}
			// donghyun : room에 자기 자신 추가 (현실 시간도)
			room.roomPartInfo[playerPtr->m_name] = { playerPtr, ServerManager::getInstance().getCurTime() };
			room.curPartCnt++;

			playerPtr->m_roomNum = room.roomNum;

			msg += std::format("**{}님이 들어오셨습니다. (현재인원 {} / {})\n\r", 
				playerPtr->m_name, room.curPartCnt, room.maxPartCnt);
			broadCastInRoom(room.roomNum, msg);
		}
	}
}

int ServerManager::getChatRoomNum(SOCKET clntfd)
{
	Player* playerPtr = findPlayerUsingfd(clntfd);
	if (playerPtr)
	{
		return playerPtr->m_roomNum;
	}
	// donghyun : 못찾았을때는 문제가 있음.
	return -2;
}

void ServerManager::broadCastChatInRoom(SOCKET clntfd, int roomNum, std::string& msg)
{
	Room& room = roomList[roomNum];
	size_t roomPartSize = room.roomPartInfo.size();

	std::string broadMsg = "";
	Player* playerPtr = findPlayerUsingfd(clntfd);
	if (playerPtr)
	{
		broadMsg += playerPtr->m_name + " > ";
		broadMsg += msg + "\n\r";

		// donghyun : 만약 개인이 채팅치던 게 있으면, 그대로 클라단에 나오게 한다.
		for (auto iter = room.roomPartInfo.begin(); iter != room.roomPartInfo.end(); iter++)
		{
			Player* playerPtr = iter->second.first;
			if (playerPtr)
			{
				std::string subMsg = playerPtr->m_buf;
				if (playerPtr->m_fd != clntfd)
				{
					if (playerPtr->m_totalStrLen != 0)
					{
						NetworkManager::getInstance().sendMsg(playerPtr->m_fd, "\n\r");
						NetworkManager::getInstance().sendMsg(playerPtr->m_fd, broadMsg);
						NetworkManager::getInstance().sendMsg(playerPtr->m_fd, subMsg.substr(0, playerPtr->m_totalStrLen));
					}
					else
					{
						NetworkManager::getInstance().sendMsg(playerPtr->m_fd, broadMsg);
					}
				}
				else
				{
					NetworkManager::getInstance().sendMsg(playerPtr->m_fd, broadMsg);
				}
			}
		}
	}
}

void ServerManager::broadCastInRoom(int roomNum, std::string& msg)
{
	Room& room = roomList[roomNum];

	for (auto iter = room.roomPartInfo.begin(); iter != room.roomPartInfo.end(); iter++)
	{
		// donghyun : 자기 자신도 브로드캐스팅함
		auto& playerInfo = iter->second;
		NetworkManager::getInstance().sendMsg(playerInfo.first->m_fd, msg);
	}
}

void ServerManager::quitPlayer(const SOCKET clntfd)
{
	Player* playerPtr = findPlayerUsingfd(clntfd);
	if (playerPtr)
	{
		if (playerPtr->m_roomNum > 0)
		{
			quitRoom(playerPtr->m_roomNum, playerPtr);
		}
		// donghyun : 플레이어 set에서도 빼기
		playerList.erase(playerPtr->m_fd);
	}
	NetworkManager::getInstance().closeClient(clntfd);
}

void ServerManager::quitRoom(const int roomNum, Player* playerPtr)
{
	Room& room = roomList[roomNum];
	room.roomPartInfo.erase(playerPtr->m_name);
	room.curPartCnt--;

	playerPtr->m_roomNum = -1;

	std::string msg = "";
	msg.reserve(100);
	msg += std::format("**{}님이 나가셨습니다. (현재인원 {} / {})\n\r",
		playerPtr->m_name, room.curPartCnt, room.maxPartCnt);
	broadCastInRoom(room.roomNum, msg);
}

// donghyun : 첫 클라 소켓 연결 요청 시에 사용됨
bool ServerManager::addPlayer(Player& player)
{
	if (playerList.find(player.m_fd) == playerList.end())
	{
		playerList[player.m_fd] = player;
		return true;
	}
	else
	{
		return false;
	}
}

// donghyun : 만약 못찾았을 때는 nullptr 반환
Player* ServerManager::findPlayerUsingfd(const SOCKET clntfd)
{
	return (playerList.find(clntfd) == playerList.end()? nullptr : &(playerList[clntfd]));
}

// donghyun : 만약 못찾았을 때는 nullptr 반환
Player* ServerManager::findPlayerUsingName(const std::string& playerName)
{
	Player* playerPtr = nullptr;
	for (auto iter = playerList.begin(); iter != playerList.end(); iter++)
	{
		if (iter->second.m_name == playerName)
		{
			playerPtr = &(iter->second);
			break;
		}
	}
	return playerPtr;
}

std::string ServerManager::getCurTime()
{
	// donghyun : HH:MM:SS 형식으로 구하는 함수
	auto now = std::chrono::system_clock::now();
	std::time_t current_time = std::chrono::system_clock::to_time_t(now);
	std::tm local_time{};
	localtime_s(&local_time, &current_time);
	char time_str[9];
	std::strftime(time_str, sizeof(time_str), "%T", &local_time);

	return time_str;
}


