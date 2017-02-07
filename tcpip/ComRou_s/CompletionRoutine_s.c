#include <stdio.h>
#include <stdlib.h>
#include <WinSock2.h>

#define BUF_SIZE 1024

//创建 两个回掉函数，这里用于读写数据。
void CALLBACK ReadCR(DWORD, DWORD, LPWSAOVERLAPPED, DWORD);
void CALLBACK WriteCR(DWORD, DWORD, LPWSAOVERLAPPED, DWORD);

void ErrorHandling(char * msg)
{
	fputs(msg, stderr);
	fputc('\n', stderr);
	system("pause");
	exit(1);
}

//创建 客户端信息结构体，这里包含着在CR函数中 收发数据的变量，该结构体是通过overlapped的hEvent参数传入
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

	//创建 overlapped结构体的指针，为什么是指针，因为要与多个客户端通信，需要相应个数的overlapped。
	//用指针来分配空间
	LPWSAOVERLAPPED lpOverlapped;
	DWORD recvBytes;
	//同理，也需要相应个数的客户端结构体
	LPIODATA clntInfo;
	//这里是修改 socket 无堵塞的变量
	int mode = 1;
	//这里是修改wsarecv接收数据的参数
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

	//创建重叠类型的socket
	servSock = WSASocket(PF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	//设置socket的属性为不堵塞
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
		//进入alertable wait时间，这样可以执行cr函数
		SleepEx(100, TRUE);
		//因为socket为不阻塞，因此调用accept时会立即返回，没有链接时返回错误 INVAILID_SOCKET，
		//并且失败号码为 WSAEWOULDBLOCK
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
		//为每个连接上的客户端创建 重叠的数据结构空间。
		lpOverlapped = (LPWSAOVERLAPPED)malloc(sizeof(WSAOVERLAPPED));
		memset(lpOverlapped, 0, sizeof(WSAOVERLAPPED));
		//为每个连接上的客户端创建 客户端信息的数据结构空间。
		clntInfo = (LPIODATA)malloc(sizeof(IODATA));
		clntInfo->hClntSock = clntSock;
		(clntInfo->wsaBuf).buf = clntInfo->buf;
		(clntInfo->wsaBuf).len = BUF_SIZE;
		//将客户端的信息 保存在 重叠结构体的hEvent中。
		lpOverlapped->hEvent = (HANDLE)clntInfo;
		//进行接收数据
		WSARecv(clntSock, &(clntInfo->wsaBuf), 1, &recvBytes, &flagInfo, lpOverlapped, ReadCR);
	}

	closesocket(clntSock);
	closesocket(servSock);
	WSACleanup();
	return 0;
}

void CALLBACK ReadCR(DWORD error, DWORD recvsz, LPWSAOVERLAPPED lpoverlapped, DWORD flags)
{
	//获取客户端的数据
	LPIODATA clntInfo = (LPIODATA)lpoverlapped->hEvent;
	SOCKET sock = clntInfo->hClntSock;
	LPWSABUF lpbufinfo = &(clntInfo->wsaBuf);
	printf("readCR:%d\n", lpbufinfo->len);
	DWORD sendBytes;
	//如果接收到的数据为0，则代表断开
	if (recvsz == 0)
	{
		//关闭socket， 清理overlapped空间，清理客户端数据空间
		closesocket(sock);
		free(lpoverlapped->hEvent);
		free(lpoverlapped);
		puts("client disconnected ... ");
	}
	else
	{
		//如果接收到数据，向客户端发送接收的数据，大小为接收到的。
		lpbufinfo->len = recvsz;
		WSASend(sock, lpbufinfo, 1, &sendBytes, 0, lpoverlapped, WriteCR);
	}

}

void CALLBACK WriteCR(DWORD error, DWORD recvsz, LPWSAOVERLAPPED lpoverlapped, DWORD flags)
{
	LPIODATA clntInfo = (LPIODATA)lpoverlapped->hEvent;
	SOCKET sock = clntInfo->hClntSock;
	LPWSABUF lpbufinfo = &(clntInfo->wsaBuf);
	printf("writeCR:%d\n", lpbufinfo->len);
	DWORD recvBytes;
	int flaginfo = 0;
	//收发一次结束之后，再次调用 接收数据，进行循环。
	//循环之前要重新设置缓冲区的大小
	lpbufinfo->len = BUF_SIZE;
	WSARecv(sock, lpbufinfo, 1, &recvBytes, &flaginfo, lpoverlapped, ReadCR);

}