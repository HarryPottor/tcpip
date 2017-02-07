#include <stdio.h>
#include <stdlib.h>
#include <WinSock2.h>

#define BUF_SIZE 1024

void ErrorHandling(char * msg)
{
	fputs(msg, stderr);
	fputc('\n', stderr);
	system("pause");
	exit(1);
}

int main(int argc, char *argv[])
{
	WSADATA wsaData;
	SOCKET servSock;
	SOCKADDR_IN servAddr;
	char message[BUF_SIZE];
	int strLen, readLen;

	if (argc != 3)
	{
		printf("usage: %s <ip><port>\n", argv[0]);
		exit(1);
	}
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		ErrorHandling("wsastartup error");
	}

	servSock = socket(PF_INET, SOCK_STREAM, 0);
	if (servSock == INVALID_SOCKET)
	{
		ErrorHandling("socket error");
	}

	memset(&servAddr, 0, sizeof(servAddr));
	servAddr.sin_family = AF_INET;
	servAddr.sin_addr.s_addr = inet_addr(argv[1]);
	servAddr.sin_port = htons(atoi(argv[2]));

	if (connect(servSock, (SOCKADDR*)&servAddr, sizeof(servAddr)) == SOCKET_ERROR)
	{
		ErrorHandling("connect error");
	}
	else
	{
		puts("connected ......");
	}

	while (1)
	{
		fputs("input message(q to quit): ", stdout);
		fgets(message, BUF_SIZE, stdin);
		if (!strcmp(message, "q\n") || !strcmp(message, "Q\n"))
		{
			break;
		}
		strLen = strlen(message);
		send(servSock, message, strLen, 0);
		readLen = 0;
		while (1)
		{
			readLen += recv(servSock, &message[readLen], BUF_SIZE - 1, 0);
			if (readLen >= strLen)
			{
				break;
			}
		}
		message[strLen] = 0;
		printf("Message from server:%s", message);
	}

	closesocket(servSock);
	WSACleanup();
	return 0;
}