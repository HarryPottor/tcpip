#include <stdio.h>
#include <stdlib.h>
#include <WinSock2.h>

#define BUF_SIZE 1024
void ErrorHandling(char * msg);

int main(int argc, char * argv[])
{
	WSADATA wsadata;
	char buf[BUF_SIZE] = {0};
	
	SOCKET servSocket, clntSocket;
	SOCKADDR_IN servAddr, clntAddr;
	int clntAddr_sz;
	int recvSize;

	WSABUF wsabuf;
	WSAEVENT wsaevent;
	WSAOVERLAPPED overlapped;

	if (argc != 2)
	{
		printf("usage:%s <port>\n", argv[0]);
		exit(1);
	}

	if (WSAStartup(MAKEWORD(2,2), &wsadata) != 0)
	{
		ErrorHandling("wsastarup error");
	}

	servSocket = WSASocket(PF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	
	memset(&servAddr, 0, sizeof(servAddr));
	servAddr.sin_family = AF_INET;
	servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servAddr.sin_port = htons(atoi(argv[1]));

	if (bind(servSocket, (SOCKADDR*)&servAddr, sizeof(servAddr)) == SOCKET_ERROR)
	{
		ErrorHandling("bind error");
	}

	if (listen(servSocket, 5) == SOCKET_ERROR)
	{
		ErrorHandling("listen error");
	}

	clntAddr_sz = sizeof(clntAddr);
	clntSocket = accept(servSocket, (SOCKADDR*)&clntAddr, &clntAddr_sz);

	wsaevent = WSACreateEvent();
	memset(&overlapped, 0, sizeof(overlapped));
	overlapped.hEvent = wsaevent;
	wsabuf.buf = buf;
	wsabuf.len = BUF_SIZE;

	int flags = 0;
	if (WSARecv(clntSocket, &wsabuf, 1, &recvSize, &flags, &overlapped, NULL) == SOCKET_ERROR)
	{
		if (WSAGetLastError() == WSA_IO_PENDING)
		{
			puts("background data receive");
			WSAWaitForMultipleEvents(1, &wsaevent, TRUE, INFINITE, FALSE);
			WSAGetOverlappedResult(clntSocket, &overlapped, &recvSize, FALSE, NULL);
		}
		else
		{
			printf("%ld\n", WSAGetLastError());
			ErrorHandling("wsarecv error");
		}
	}

	printf("receive message: %s\n", buf);
	WSACloseEvent(wsaevent);
	closesocket(servSocket);
	closesocket(clntSocket);
	WSACleanup();

	return 0;
}


void ErrorHandling(char * msg)
{
	fputs(msg, stderr);
	fputc('\n', stderr);
	system("pause");
	exit(1);
}