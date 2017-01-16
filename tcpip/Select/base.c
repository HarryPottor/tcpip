#ifdef DEBUG

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <WinSock2.h>

#define BUF_SIZE 1024
#define SA SOCKADDR

void showerror(char * msg);

int main123(int argc, char* argv[])
{
	WSADATA wsadata;
	SOCKET servsock, clntsock;
	SOCKADDR_IN servaddr, clntaddr;
	int clntaddrsz;

	char buf[BUF_SIZE];
	int readlen;

	if (argc != 2)
	{
		showerror("usage: exe <port>");
	}
	if (WSAStartup(MAKEWORD(2, 2), &wsadata) != 0)
	{
		showerror("wsastartup error");
	}
	servsock = socket(PF_INET, SOCK_STREAM, 0);
	if (servsock == INVALID_SOCKET)
	{
		showerror("socket error");
	}

	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(atoi(argv[1]));

	if (bind(servsock, (SA*)&servaddr, sizeof(servaddr)) == SOCKET_ERROR)
	{
		showerror("bind error");
	}

	if (listen(servsock, 5) == SOCKET_ERROR)
	{
		showerror("listen error");
	}

	while (1)
	{
		clntaddrsz = sizeof(clntaddr);
		clntsock = accept(servsock, (SA*)&clntaddr, &clntaddrsz);
		while (1)
		{
			readlen = recv(clntsock, buf, BUF_SIZE - 1, 0);
			if (readlen == 0)
			{
				break;
			}
			buf[readlen] = '\0';
			send(clntsock, buf, readlen, 0);
		}

		closesocket(clntsock);
	}

	closesocket(servsock);
	WSACleanup();
	return 0;
}

void showerror(char *msg)
{
	fputs(msg, stderr);
	fputc('\n', stderr);
	system("pause");
	exit(1);
}

#endif // DEBUG