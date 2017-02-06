#include <stdio.h>
#include <stdlib.h>
#include <WinSock2.h>

#define BUF_SIZE 1024
void ErrorHandling(char * msg);

void CALLBACK ComRoutine(DWORD, DWORD, LPWSAOVERLAPPED, DWORD);

typedef struct 
{
	int arg;
	int age;
}SData;


WSABUF wsabuf;
char buf[BUF_SIZE] = { 0 };
int recvSize;
int main(int argc, char * argv[])
{
	WSADATA wsadata;
	
	SOCKET servSocket, clntSocket;
	SOCKADDR_IN servAddr, clntAddr;
	int clntAddr_sz;
	
	WSAEVENT wsaevent;
	WSAOVERLAPPED overlapped;

	if (argc != 2)
	{
		printf("usage:%s <port>\n", argv[0]);
		exit(1);
	}

	if (WSAStartup(MAKEWORD(2, 2), &wsadata) != 0)
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
	//用CR验证时 无需设置event，之所以创建event是为了调用 WSAWaitForMultipleEvents 函数进入alertable wait状态
	//overlapped.hEvent = wsaevent;
	
	wsabuf.buf = buf;
	wsabuf.len = BUF_SIZE;

	SData sdata;
	sdata.age = 10;
	sdata.arg = 20;
	overlapped.hEvent = (HANDLE)&sdata;

	int flags = 0;
	//这里的overlapped实际上是可以作为 参数 传递给 CR函数的。
	if (WSARecv(clntSocket, &wsabuf, 1, &recvSize, &flags, &overlapped, ComRoutine) == SOCKET_ERROR)
	{
		if (WSAGetLastError() == WSA_IO_PENDING)
		{
			puts("background data receive");
		}
	}
	//调用 WSAWaitForMultipleEvents 函数进入alertable wait状态
	int idx = WSAWaitForMultipleEvents(1, &wsaevent, FALSE, INFINITE, TRUE);

	if (idx == WAIT_IO_COMPLETION)
	{
		puts("Overlapped complete");
	}
	else
	{
		ErrorHandling("wsarecv error");
	}

	WSACloseEvent(wsaevent);
	closesocket(servSocket);
	closesocket(clntSocket);
	WSACleanup();

	system("pause");
	return 0;
}


void ErrorHandling(char * msg)
{
	fputs(msg, stderr);
	fputc('\n', stderr);
	system("pause");
	exit(1);
}

void CALLBACK ComRoutine(DWORD error, DWORD recvsz, LPWSAOVERLAPPED lpOverlapped, DWORD flags)
{
	
	if (error != 0)
	{
		ErrorHandling("CR error");
	}
	else
	{
		SData *sd = (SData*)(lpOverlapped->hEvent);
		printf("age = %d, arg = %d\n", sd->age, sd->arg);
		recvSize = recvsz;
		printf("receive message: %s\n", buf);
	}
}