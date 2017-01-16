
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <WinSock2.h>

#define BUF_SIZE 1024
#define SA SOCKADDR

void showerror(char * msg);

int main(int argc, char* argv[])
{
	WSADATA wsadata;
	SOCKET servsock;
	SOCKADDR_IN servaddr;

	char buf[BUF_SIZE];
	int readlen;

	if (argc != 3)
	{
		showerror("usage: exe <ip><port>");
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
	servaddr.sin_addr.s_addr = inet_addr(argv[1]);
	servaddr.sin_port = htons(atoi(argv[2]));

	if (connect(servsock, (SA*)&servaddr, sizeof(servaddr)) == SOCKET_ERROR)
	{
		showerror("connect error");
	}
	while (1)
	{
		
		fgets(buf, BUF_SIZE - 1, stdin);
		if (!strcmp(buf, "quit\n") || !strcmp(buf, "end\n") || !strcmp(buf, "bye\n"))
		{
			break;
		}
		int sendlen = send(servsock, buf, strlen(buf), 0);
		int readcnt = 0;
		int readlen = 0;
		while (readlen < sendlen)
		{
			readcnt = recv(servsock, buf, BUF_SIZE - 1, 0);
			buf[readcnt] = '\0';
			readlen += readcnt;
			printf("receive from server:%s\n", buf);
		}
		
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

