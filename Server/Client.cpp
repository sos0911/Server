#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <WinSock2.h>

#define BUF_SIZE 1024
#define RLT_SIZE 4
#define OPSZ 4

void ErrorHandling(const char* message);

//int main(int argc, char* argv[])
//{
//	WSADATA wsadata;
//	SOCKET hSocket;
//	char opmsg[BUF_SIZE];
//	int result, opndCnt, i;
//	SOCKADDR_IN servAdr;
//	if (argc != 3)
//	{
//		printf("usage : %s <IP> <port>\n", argv[0]);
//		exit(1);
//	}
//
//	if (WSAStartup(MAKEWORD(2, 2), &wsadata) != 0)
//	{
//		ErrorHandling("WSAstartup() error");
//	}
//
//	hSocket = socket(PF_INET, SOCK_STREAM, 0);
//	if (hSocket == INVALID_SOCKET)
//	{
//		ErrorHandling("socket() error");
//	}
//
//	memset(&servAdr, 0, sizeof(servAdr));
//	servAdr.sin_family = AF_INET;
//	servAdr.sin_addr.s_addr = inet_addr(argv[1]);
//	servAdr.sin_port = htons(atoi(argv[2]));
//
//	if (connect(hSocket, (SOCKADDR*)&servAdr, sizeof(servAdr)) == SOCKET_ERROR)
//	{
//		ErrorHandling("connect() error");
//	}
//	else
//	{
//		puts("connected............");
//	}
//
//	fputs("Operand count : ", stdout);
//	scanf("%d", &opndCnt);
//	opmsg[0] = (char)opndCnt;
//
//	for (i = 0; i < opndCnt; i++)
//	{
//		printf("Operand %d : ", i + 1);
//		scanf("%d", (int*)&opmsg[i * OPSZ + 1]);
//	}
//	fgetc(stdin);
//	fputs("Operator : ", stdout);
//	scanf("%c", &opmsg[opndCnt * OPSZ + 1]);
//	send(hSocket, opmsg, opndCnt * OPSZ + 2, 0);
//	recv(hSocket, (char*)&result, RLT_SIZE, 0);
//
//	printf("Operation result : %d\n", result);
//	closesocket(hSocket);
//	WSACleanup();
//	return 0;
//}

void ErrorHandling(const char* message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}

// 프로그램 실행: <Ctrl+F5> 또는 [디버그] > [디버깅하지 않고 시작] 메뉴
// 프로그램 디버그: <F5> 키 또는 [디버그] > [디버깅 시작] 메뉴

// 시작을 위한 팁: 
//   1. [솔루션 탐색기] 창을 사용하여 파일을 추가/관리합니다.
//   2. [팀 탐색기] 창을 사용하여 소스 제어에 연결합니다.
//   3. [출력] 창을 사용하여 빌드 출력 및 기타 메시지를 확인합니다.
//   4. [오류 목록] 창을 사용하여 오류를 봅니다.
//   5. [프로젝트] > [새 항목 추가]로 이동하여 새 코드 파일을 만들거나, [프로젝트] > [기존 항목 추가]로 이동하여 기존 코드 파일을 프로젝트에 추가합니다.
//   6. 나중에 이 프로젝트를 다시 열려면 [파일] > [열기] > [프로젝트]로 이동하고 .sln 파일을 선택합니다.
