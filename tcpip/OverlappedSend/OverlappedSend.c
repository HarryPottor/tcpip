#include <stdio.h>
#include <stdlib.h>
#include <WinSock2.h>

void ErrorHandling(char * msg);

int main(int argc, char *argv[])
{
	WSADATA wsaData;
	SOCKET hSocket;
	SOCKADDR_IN sendAddr;

	//wsasend �ĵڶ������������ݵ�buf
	WSABUF dataBuf;
	char msg[] = "Newwork is computer!";
	int sendBytes = 0;
	//�����¼�����overlapped�Ľṹ��
	WSAEVENT eventObj;
	WSAOVERLAPPED overlapped;

	if (argc != 3)
	{
		printf("usage: %s <ip> <port>\n", argv[0]);
		exit(1);
	}

	if (WSAStartup(MAKEWORD(2,2), &wsaData) != 0)
	{
		ErrorHandling("wsastartup error");
	}

	hSocket = WSASocket(PF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	memset(&sendAddr, 0, sizeof(sendAddr));
	sendAddr.sin_family = AF_INET;
	sendAddr.sin_addr.s_addr = inet_addr(argv[1]);
	sendAddr.sin_port = htons(atoi(argv[2]));

	if (connect(hSocket, (SOCKADDR*)&sendAddr, sizeof(sendAddr)) == SOCKET_ERROR)
	{
		ErrorHandling("connect error");
	}

	//�����¼�
	eventObj = WSACreateEvent();
	//��� overlapped�ṹ��
	memset(&overlapped, 0, sizeof(overlapped));
	overlapped.hEvent = eventObj;
	//��� ����buf�ṹ��
	dataBuf.buf = msg;
	dataBuf.len = strlen(msg) + 1;

	//����wsasend����
	if (WSASend(hSocket, &dataBuf, 1, &sendBytes, 0, &overlapped, NULL) == SOCKET_ERROR)
	{
		if (WSAGetLastError() == WSA_IO_PENDING)
		{
			puts("Background data send");
			WSAWaitForMultipleEvents(1, &eventObj, TRUE, INFINITE, FALSE);
			WSAGetOverlappedResult(hSocket, &overlapped, &sendBytes, FALSE, NULL);
		}
		else
		{
			ErrorHandling("wsasend error");
		}
	}

	printf("Send data size: %d\n", sendBytes);
	WSACloseEvent(eventObj);
	closesocket(hSocket);
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