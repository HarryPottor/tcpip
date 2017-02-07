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
	// -------------------------------------------------------------------------------------------------------���� ��ɶ˿�
	cp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
	GetSystemInfo(&sysInfo);
	for (size_t i = 0; i < sysInfo.dwNumberOfProcessors ; i++)
	{
		//--------------------------------------------------------------------------------------------------���� �̴߳���IO
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
		//-----------------------------------------------------------------------------���� �׽������ݿռ�
		lpsockinfo = (LPSockInfo)malloc(sizeof(SockInfo));
		lpsockinfo->clntSock = clntSock;
		memcpy(&(lpsockinfo->clntAddr), &clntAddr, addrLen);
		//lpsockinfo->clntAddr = clntAddr;
		//ע�� ������ ULONG_PTR
		//-------------------------------------------------------------------------------------------------���� ��ɶ˿� �� �׽��֣� ���Ҵ����׽��ֵ�����
		CreateIoCompletionPort((HANDLE)clntSock, cp, (ULONG_PTR)lpsockinfo, 0);

		//��һ�ν�������======================
		//-----------------------------------------------------------------------------���� IO���� �ṹ�ռ�
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
		//Ӧ�ö�������� ���io��ɡ�
		//ע�� �� ������ PULONG_PTR 
		//--------------------------------------------------------------------------------------------------���� IO���
		//ע�� ���ĸ����� (LPOVERLAPPED*)&lpioinfo����Ϊ�ṹ���׵�ַ == �ṹ���һ�������ĵ�ַ�����Կ�����LPIOInfo�ĵ�ַ����OVERLAPPED�ĵ�ַ
		GetQueuedCompletionStatus(cp, &bytesTrans, (PULONG_PTR)&lpsockinfo,
			(LPOVERLAPPED*)&lpioinfo, INFINITE);
		sock = lpsockinfo->clntSock;
		//���io��ɵ��� READ ģʽ�������ȡ���
		if (lpioinfo->mode == READ)
		{
			puts("message receive");
			//�������ֽ�Ϊ0�������ͻ��˹ر�
			if (bytesTrans == 0)
			{
				closesocket(sock);
				free(lpsockinfo);
				free(lpioinfo);
				continue;
			}
			//�����ȡ�����ݣ���ԭ������ȥ
			memset(&(lpioinfo->overlapped), 0, sizeof(OVERLAPPED));
			lpioinfo->wsaBuf.len = bytesTrans;
			lpioinfo->mode = WRITE;
			WSASend(sock, &(lpioinfo->wsaBuf), 1, NULL, 0, &(lpioinfo->overlapped), NULL);

			//Ȼ���ٴ�׼����������===================
			lpioinfo = (LPIOInfo)malloc(sizeof(IOInfo));
			memset(&(lpioinfo->overlapped), 0, sizeof(IOInfo));
			lpioinfo->wsaBuf.len = BUF_SIZE;
			lpioinfo->wsaBuf.buf = lpioinfo->buf;
			lpioinfo->mode = READ;
			WSARecv(sock, &(lpioinfo->wsaBuf), 1, NULL, &flags,
				&(lpioinfo->overlapped), NULL);
		}
		else //�����ɵ��� WRITE ģʽ�����������ɡ�
		{
			puts("message sent");
			free(lpioinfo);
		}

	}
	return 0;
}
