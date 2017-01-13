#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>

#define SA SOCKADDR
#define BUF_SZ 100
#define OPSZ 4
int main(int argc, char * argv[])
{
	WSADATA wsaData;
	SOCKET servSock, clntSock;
	SOCKADDR_IN servAddr, clntAddr;
	int clntAddrSz;

	char buf[BUF_SZ];

	WSAStartup(MAKEWORD(2,2), &wsaData);

	servSock = socket(PF_INET, SOCK_STREAM, 0);

	memset(&servAddr, 0, sizeof(servAddr));
	servAddr.sin_family = AF_INET;
	servAddr.sin_addr.s_addr = inet_addr(argv[1]);
	servAddr.sin_port = htons(atoi(argv[2]));

	connect(servSock, (SA*)&servAddr, sizeof(servAddr));

	int count;
	printf("input count:");
	scanf("%d", &count);

	buf[0] = *((char*)&count);

	for (size_t i = 0; i < count; i++)
	{
		printf("input %dth:", i+1);
		scanf("%d", (int*)&buf[i*OPSZ + 1]);
	}
	fgetc(stdin);
	printf("input operator:");
	scanf("%c", &buf[count*OPSZ + 1]);

	send(servSock, buf, count*OPSZ + 2, 0);
	int result;
	recv(servSock, (char*)&result, sizeof(result), 0);

	printf("result:%d\n", result);

	closesocket(servSock);
	WSACleanup();
	system("pause");
	return 0;
}