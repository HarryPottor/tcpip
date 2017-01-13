#include <stdio.h>
#include <winsock2.h>
#include <stdlib.h>

void showerror(char * msg);

int main(int argc, char * argv[])
{
	SOCKET hSocket;
	WSADATA wsaData;
	SOCKADDR_IN servAddr;

	char msg[30];
	int strLen = 0;
	int idx = 0, readLen = 0;

	if (argc != 3)
	{
		showerror("usage: exe <ip> <port>");
	}

	if (WSAStartup(MAKEWORD(2,2), &wsaData) != 0)
	{
		showerror("wsastartup error");
	}

	hSocket = socket(PF_INET, SOCK_STREAM, 0);
	if (hSocket == INVALID_SOCKET)
	{
		showerror("socket error");
	}

	memset(&servAddr, 0, sizeof(servAddr));
	servAddr.sin_family = AF_INET;
	servAddr.sin_addr.s_addr = inet_addr(argv[1]);
	servAddr.sin_port = htons(atoi(argv[2]));

	if (SOCKET_ERROR == (connect(hSocket, (SOCKADDR*)&servAddr, sizeof(servAddr))))
	{
		showerror("connect error");
	}

	while (readLen = recv(hSocket, &msg[idx++], 1, 0))
	{
		printf("%d\t", readLen);
		if (readLen == -1)
			showerror("read error");
		strLen += readLen;
	}
	printf("%d\n", readLen);
	msg[strLen] = '\0';
	printf("msg from server: %s \n", msg);
	printf("read call counts: %d \n", strLen);

	closesocket(hSocket);
	WSACleanup();
	system("pause");
	return 0;
}

void showerror(char * msg)
{
	fputs(msg, stderr);
	fputc('\n', stderr);
	system("pause");
	exit(1);
}