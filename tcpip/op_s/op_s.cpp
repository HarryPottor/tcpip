#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>

#define SA SOCKADDR
#define BUF_SZ 100
#define OPSZ 4

int calculate(int count, int num[], char op);
int main(int argc, char * argv[])
{
	WSADATA wsaData;
	SOCKET servSock, clntSock;
	SOCKADDR_IN servAddr, clntAddr;
	int clntAddrSz;

	char buf[BUF_SZ];

	WSAStartup(MAKEWORD(2, 2), &wsaData);

	servSock = socket(PF_INET, SOCK_STREAM, 0);

	memset(&servAddr, 0, sizeof(servAddr));
	servAddr.sin_family = AF_INET;
	servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servAddr.sin_port = htons(atoi(argv[1]));

	bind(servSock, (SA*)&servAddr, sizeof(servAddr));

	listen(servSock, 5);
	int count = 0;
	while (true)
	{
		clntAddrSz = sizeof(clntAddr);
		clntSock = accept(servSock, (SA*)&clntAddr, &clntAddrSz);

		recv(clntSock, (char*)&count, 1, 0);
		printf("count = %d\n", count);
		int readLen = 0;
		int readCnt = 0;
		while (readLen < count*OPSZ+1)
		{
			readCnt = recv(clntSock, &buf[readLen], BUF_SZ - 1, 0);
			printf("----------------%d\n", readCnt);
			readLen += readCnt;
		}
		int result = calculate(count, (int*)buf, buf[readLen-1]);

		send(clntSock, (char*)&result, sizeof(result), 0);
		closesocket(clntSock);
	}
	
	closesocket(servSock);
	WSACleanup();

	return 0;
}

int calculate(int count, int num[], char op)
{
	int result = num[0];
	switch (op)
	{
	case '+':
		for (size_t i = 1; i < count; i++)
		{
			result += num[i];
		}
		break;
	case '-':
		for (size_t i = 1; i < count; i++)
		{
			result -= num[i];
		}
		break;
	case '*':
		for (size_t i = 1; i < count; i++)
		{
			result *= num[i];
		}
		break;
	default:
		break;
	}

	return result;
}