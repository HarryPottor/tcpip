#include <stdio.h>
#include <stdlib.h>
#include <process.h>
#include <WinSock2.h>
#include <Windows.h>

#define BUF_SIZE 100
#define READ 3
#define WRITE 5

typedef struct
{
	SOCKET clntSock;
	SOCKADDR_IN clntAddr;
}SockInfo, *LPSockInfo;

typedef struct
{
	OVERLAPPED overlapped;
	WSABUF wsaBuf;
	char buf[BUF_SIZE];
	int mode;
} IOInfo, * LPIOInfo;

DWORD WINAPI ThreadMain(LPVOID CP);
void ErrorHandling(char * msg)
{
	fputs(msg, stderr);
	fputc('\n', stderr);
	system("pause");
	exit(1);
}


int main(int argc, char* argv[])
{
	WSADATA wsaData;
	SOCKET servSock;
	SOCKADDR_IN servAddr; 
	int recvBytes, flag = 0;

	HANDLE cp;
	SYSTEM_INFO sysInfo;
	LPIOInfo lpioinfo;
	LPSockInfo lpsockinfo;

	if (argc != 2)
	{
		printf("usage: %s <port>\n", argv[0]);
		exit(1);
	}

	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		ErrorHandling("wsastartup error");
	}
	// -------------------------------------------------------------------------------------------------------创建 完成端口
	cp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
	GetSystemInfo(&sysInfo);
	for (size_t i = 0; i < sysInfo.dwNumberOfProcessors ; i++)
	{
		//--------------------------------------------------------------------------------------------------创建 线程处理IO
		_beginthreadex(NULL, 0, ThreadMain, (LPVOID)cp, 0, NULL);
	}

	servSock = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);

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

	while (1)
	{
		SOCKET clntSock;
		SOCKADDR_IN clntAddr;
		int addrLen = sizeof(clntAddr);

		clntSock = accept(servSock, (SOCKADDR*)&clntAddr, &addrLen);
		//-----------------------------------------------------------------------------创建 套接字数据空间
		lpsockinfo = (LPSockInfo)malloc(sizeof(SockInfo));
		lpsockinfo->clntSock = clntSock;
		memcpy(&(lpsockinfo->clntAddr), &clntAddr, addrLen);
		//lpsockinfo->clntAddr = clntAddr;
		//注意 这里用 ULONG_PTR
		//-------------------------------------------------------------------------------------------------链接 完成端口 与 套接字， 并且传入套接字的数据
		CreateIoCompletionPort((HANDLE)clntSock, cp, (ULONG_PTR)lpsockinfo, 0);

		//第一次接收数据======================
		//-----------------------------------------------------------------------------创建 IO传输 结构空间
		lpioinfo = (LPIOInfo)malloc(sizeof(IOInfo));
		memset(&(lpioinfo->overlapped), 0, sizeof(OVERLAPPED));
		lpioinfo->wsaBuf.buf = lpioinfo->buf;
		lpioinfo->wsaBuf.len = BUF_SIZE;
		lpioinfo->mode = READ;
		WSARecv(lpsockinfo->clntSock, &(lpioinfo->wsaBuf), 
			1, &recvBytes, &flag, &(lpioinfo->overlapped), NULL);
	}
	return 0;
}

DWORD WINAPI ThreadMain(LPVOID ComPort)
{
	HANDLE cp = (HANDLE)ComPort;
	SOCKET sock;
	DWORD bytesTrans;
	LPSockInfo lpsockinfo;
	LPIOInfo lpioinfo;
	DWORD flags = 0;

	while (1)
	{
		//应该堵塞在这里， 监测io完成。
		//注意 ： 这里用 PULONG_PTR 
		//--------------------------------------------------------------------------------------------------监听 IO完成
		//注意 第四个参数 (LPOVERLAPPED*)&lpioinfo，因为结构的首地址 == 结构体第一个变量的地址，所以可以用LPIOInfo的地址保存OVERLAPPED的地址
		GetQueuedCompletionStatus(cp, &bytesTrans, (PULONG_PTR)&lpsockinfo,
			(LPOVERLAPPED*)&lpioinfo, INFINITE);
		sock = lpsockinfo->clntSock;
		//如果io完成的是 READ 模式，代表读取完成
		if (lpioinfo->mode == READ)
		{
			puts("message receive");
			//如果获得字节为0，则代表客户端关闭
			if (bytesTrans == 0)
			{
				closesocket(sock);
				free(lpsockinfo);
				free(lpioinfo);
				continue;
			}
			//如果获取到数据，则原样发回去
			memset(&(lpioinfo->overlapped), 0, sizeof(OVERLAPPED));
			lpioinfo->wsaBuf.len = bytesTrans;
			lpioinfo->mode = WRITE;
			WSASend(sock, &(lpioinfo->wsaBuf), 1, NULL, 0, &(lpioinfo->overlapped), NULL);

			//然后再次准备接收数据===================
			lpioinfo = (LPIOInfo)malloc(sizeof(IOInfo));
			memset(&(lpioinfo->overlapped), 0, sizeof(IOInfo));
			lpioinfo->wsaBuf.len = BUF_SIZE;
			lpioinfo->wsaBuf.buf = lpioinfo->buf;
			lpioinfo->mode = READ;
			WSARecv(sock, &(lpioinfo->wsaBuf), 1, NULL, &flags,
				&(lpioinfo->overlapped), NULL);
		}
		else //如果完成的是 WRITE 模式，则代表发送完成。
		{
			puts("message sent");
			free(lpioinfo);
		}

	}
	return 0;
}
