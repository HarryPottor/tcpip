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
	SOCKET servsock, clntsock;
	SOCKADDR_IN servaddr, clntaddr;
	int clntaddrsz;

	char buf[BUF_SIZE];
	int readlen;

	



	if (argc != 2)
	{
		showerror("usage: exe <port>");
	}
	if (WSAStartup(MAKEWORD(2,2), &wsadata) != 0)
	{
		showerror("wsastartup error");
	}
	servsock = socket(PF_INET, SOCK_STREAM, 0);
	if (servsock == INVALID_SOCKET)
	{
		showerror("socket error");
	}

	//selectµÄ²Ù×÷
	fd_set readset, tempset;
	FD_ZERO(&readset);
	FD_SET(servsock, &readset);
	struct timeval timeout;


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
		tempset = readset;
		timeout.tv_sec = 5;
		timeout.tv_usec = 0;
		int result = select(0, &tempset, 0, 0, &timeout);
		if (result == SOCKET_ERROR)
		{
			puts("select error");
			break;
		}
		else if(result == 0)
		{
			puts("time out");
			continue;
		}
		else
		{
			for (size_t i = 0; i < readset.fd_count; i++)
			{
				if (FD_ISSET(readset.fd_array[i], &tempset))
				{
					if (readset.fd_array[i] == servsock)
					{
						clntaddrsz = sizeof(clntaddr);
						clntsock = accept(servsock, (SA*)&clntaddr, &clntaddrsz);
						FD_SET(clntsock, &readset);
					}
					else
					{
						readlen = recv(readset.fd_array[i], buf, BUF_SIZE - 1, 0);
						if (readlen == 0)
						{
							FD_CLR(readset.fd_array[i], &readset);
							closesocket(readset.fd_array[i]);
						}
						else
						{
							buf[readlen] = '\0';
							send(readset.fd_array[i], buf, readlen, 0);
						}
					}
				}
			}
		}
		
		
		
		
	}

	closesocket(servsock);
	WSACleanup();
	system("pause");
	return 0;
}

void showerror(char *msg)
{
	fputs(msg, stderr);
	fputc('\n', stderr);
	system("pause");
	exit(1);
}