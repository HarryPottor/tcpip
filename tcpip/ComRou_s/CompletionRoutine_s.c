#include <stdio.h>
#include <stdlib.h>
#include <WinSock2.h>

#define BUF_SIZE 1024

void CALLBACK ReadCR(DWORD, DWORD, LPWSAOVERLAPPED, DWORD);
void CALLBACK WriteCR(DWORD, DWORD, LPWSAOVERLAPPED, DWORD);

void ErrorHandling(char * msg)
{
	fputs(msg, stderr);
	fputc('\n', stderr);
	system("pause");
	exit(1);
}


typedef struct
{
	SOCKET hClntSock;
	char buf[BUF_SIZE];
	WSABUF wsaBuf;
}IODATA, * LPIODATA;

int main(int argc, char *argv[])
{
	WSADATA wsaData;
	SOCKET servSock, clntSock;
	SOCKADDR_IN servAddr, clntAddr;
	int clntAddr_sz;

	LPWSAOVERLAPPED lpOverlapped;
	DWORD recvBytes;
	LPIODATA clntInfo;

	int mode = 1;
	int flagInfo = 0;

	if (argc != 2)
	{
		printf("usage: %s <port>\n", argv[0]);
		exit(1);
	}

	if (WSAStartup(MAKEWORD(2,2), &wsaData) != 0)
	{
		ErrorHandling("wsastartup error");
	}

	servSock = WSASocket(PF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	ioctlsocket(servSock, FIONBIO, &mode);

	memset(&servAddr, 0, sizeof(servAddr));
	servAddr.sin_family = AF_INET;
	servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servAddr.sin_port = htons(atoi(argv[1]));

	if (bind(servSock, (SOCKADDR*)&servAddr, sizeof(servAddr)) == SOCKET_ERROR)
	{
		ErrorHandling("bind error");
	}

	if (listen(servSock, 5) == SOCKET_ERROR)
	{
		ErrorHandling("listen error");
	}

	clntAddr_sz = sizeof(clntAddr);

	while (1)
	{
		SleepEx(100, TRUE);

		clntSock = accept(servSock, (SOCKADDR*)&clntAddr, &clntAddr_sz);
		if (clntSock == INVALID_SOCKET)
		{
			if (WSAGetLastError() == WSAEWOULDBLOCK)
			{
				continue;
			}
			else
			{
				ErrorHandling("accept error");
			}
		}
		puts("clinet connected");
		
		lpOverlapped = (LPWSAOVERLAPPED)malloc(sizeof(WSAOVERLAPPED));
		memset(lpOverlapped, 0, sizeof(WSAOVERLAPPED));
		
		clntInfo = (LPIODATA)malloc(sizeof(IODATA));
		clntInfo->hClntSock = clntSock;
		(clntInfo->wsaBuf).buf = clntInfo->buf;
		(clntInfo->wsaBuf).len = BUF_SIZE;

		lpOverlapped->hEvent = (HANDLE)clntInfo;
		WSARecv(clntSock, &(clntInfo->wsaBuf), 1, &recvBytes, &flagInfo, lpOverlapped, ReadCR);
	}

	closesocket(clntSock);
	closesocket(servSock);
	WSACleanup();
	return 0;
}