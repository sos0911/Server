#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <WinSock2.h>

#define BUF_SIZE 1024
#define OPSZ 4

void ErrorHandling(const char* message);
int Calculate(int opnum, int opnds[], char op);

//int main(int argc, char* argv[])
//{
//	WSADATA wsadata;
//	SOCKET hServsock, hClntSock;
//	char opinfo[BUF_SIZE];
//	int result, opndCnt, i;
//	int recvCnt, recvLen;
//	SOCKADDR_IN servAdr, clntAdr;
//	int clneAdrSize;
//	if (argc != 2)
//	{
//		printf("usage : %s <port>\n", argv[0]);
//		exit(1);
//	}
//
//	if (WSAStartup(MAKEWORD(2, 2), &wsadata) != 0)
//	{
//		ErrorHandling("WSAstartup() error");
//	}
//
//	hServsock = socket(PF_INET, SOCK_STREAM, 0);
//	if (hServsock == INVALID_SOCKET)
//	{
//		ErrorHandling("socket() error");
//	}
//
//	memset(&servAdr, 0, sizeof(servAdr));
//	servAdr.sin_family = AF_INET;
//	servAdr.sin_addr.s_addr = htonl(INADDR_ANY);
//	servAdr.sin_port = htons(atoi(argv[1]));
//
//	if (bind(hServsock, (sockaddr*)&servAdr, sizeof(servAdr)) == SOCKET_ERROR)
//	{
//		ErrorHandling("bind() error");
//	}
//	if (listen(hServsock, 5) == SOCKET_ERROR)
//	{
//		ErrorHandling("listen() error");
//	}
//
//	clneAdrSize = sizeof(clntAdr);
//
//	for (i = 0; i < 5; i++)
//	{
//		opndCnt = 0;
//		hClntSock = accept(hServsock, (SOCKADDR*)&clntAdr, &clneAdrSize);
//		recv(hClntSock, (char*)&opndCnt, 1, 0);
//
//		recvLen = 0;
//		while ((opndCnt * OPSZ + 1) > recvLen)
//		{
//			recvCnt = recv(hClntSock, &opinfo[recvLen], BUF_SIZE - 1, 0);
//			recvLen += recvCnt;
//		}
//		result = Calculate(opndCnt, (int*)opinfo, opinfo[recvLen - 1]);
//		send(hClntSock, (char*)&result, sizeof(result), 0);
//		closesocket(hClntSock);
//	}
//	closesocket(hServsock);
//	WSACleanup();
//	return 0;
//}

void ErrorHandling(const char* message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}

int Calculate(int opnum, int opnds[], char op)
{
	int result = opnds[0], i;

	switch (op)
	{
	case '+':
		for (i = 1; i < opnum; i++)
		{
			result += opnds[i];
			break;
		}
	case '-':
		for (i = 1; i < opnum; i++)
		{
			result -= opnds[i];
			break;
		}
	case '*':
		for (i = 1; i < opnum; i++)
		{
			result *= opnds[i];
			break;
		}
	}
	return result;
}
