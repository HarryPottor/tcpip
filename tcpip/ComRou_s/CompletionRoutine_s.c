#include <stdio.h>
#include <stdlib.h>
#include <WinSock2.h>

#define BUF_SIZE 1024

//���� �����ص��������������ڶ�д���ݡ�
void CALLBACK ReadCR(DWORD, DWORD, LPWSAOVERLAPPED, DWORD);
void CALLBACK WriteCR(DWORD, DWORD, LPWSAOVERLAPPED, DWORD);

void ErrorHandling(char * msg)
{
	fputs(msg, stderr);
	fputc('\n', stderr);
	system("pause");
	exit(1);
}

//���� �ͻ�����Ϣ�ṹ�壬�����������CR������ �շ����ݵı������ýṹ����ͨ��overlapped��hEvent��������
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

	//���� overlapped�ṹ���ָ�룬Ϊʲô��ָ�룬��ΪҪ�����ͻ���ͨ�ţ���Ҫ��Ӧ������overlapped��
	//��ָ��������ռ�
	LPWSAOVERLAPPED lpOverlapped;
	DWORD recvBytes;
	//ͬ��Ҳ��Ҫ��Ӧ�����Ŀͻ��˽ṹ��
	LPIODATA clntInfo;
	//�������޸� socket �޶����ı���
	int mode = 1;
	//�������޸�wsarecv�������ݵĲ���
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

	//�����ص����͵�socket
	servSock = WSASocket(PF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	//����socket������Ϊ������
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
		//����alertable waitʱ�䣬��������ִ��cr����
		SleepEx(100, TRUE);
		//��ΪsocketΪ����������˵���acceptʱ���������أ�û������ʱ���ش��� INVAILID_SOCKET��
		//����ʧ�ܺ���Ϊ WSAEWOULDBLOCK
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
		//Ϊÿ�������ϵĿͻ��˴��� �ص������ݽṹ�ռ䡣
		lpOverlapped = (LPWSAOVERLAPPED)malloc(sizeof(WSAOVERLAPPED));
		memset(lpOverlapped, 0, sizeof(WSAOVERLAPPED));
		//Ϊÿ�������ϵĿͻ��˴��� �ͻ�����Ϣ�����ݽṹ�ռ䡣
		clntInfo = (LPIODATA)malloc(sizeof(IODATA));
		clntInfo->hClntSock = clntSock;
		(clntInfo->wsaBuf).buf = clntInfo->buf;
		(clntInfo->wsaBuf).len = BUF_SIZE;
		//���ͻ��˵���Ϣ ������ �ص��ṹ���hEvent�С�
		lpOverlapped->hEvent = (HANDLE)clntInfo;
		//���н�������
		WSARecv(clntSock, &(clntInfo->wsaBuf), 1, &recvBytes, &flagInfo, lpOverlapped, ReadCR);
	}

	closesocket(clntSock);
	closesocket(servSock);
	WSACleanup();
	return 0;
}

void CALLBACK ReadCR(DWORD error, DWORD recvsz, LPWSAOVERLAPPED lpoverlapped, DWORD flags)
{
	//��ȡ�ͻ��˵�����
	LPIODATA clntInfo = (LPIODATA)lpoverlapped->hEvent;
	SOCKET sock = clntInfo->hClntSock;
	LPWSABUF lpbufinfo = &(clntInfo->wsaBuf);
	printf("readCR:%d\n", lpbufinfo->len);
	DWORD sendBytes;
	//������յ�������Ϊ0�������Ͽ�
	if (recvsz == 0)
	{
		//�ر�socket�� ����overlapped�ռ䣬����ͻ������ݿռ�
		closesocket(sock);
		free(lpoverlapped->hEvent);
		free(lpoverlapped);
		puts("client disconnected ... ");
	}
	else
	{
		//������յ����ݣ���ͻ��˷��ͽ��յ����ݣ���СΪ���յ��ġ�
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
	//�շ�һ�ν���֮���ٴε��� �������ݣ�����ѭ����
	//ѭ��֮ǰҪ�������û������Ĵ�С
	lpbufinfo->len = BUF_SIZE;
	WSARecv(sock, lpbufinfo, 1, &recvBytes, &flaginfo, lpoverlapped, ReadCR);

}