
#include <stdio.h>
#include <winsock2.h>
#include <stdlib.h>
#include <iostream>
using namespace std;

#define MAXLEN 100
void showError(char * msg)
{
	cout << msg << endl;
	system("pause");
	exit(1);
}

int main(int argc, char *argv[])
{
	if (argc != 2)
	{
		cout << "usage:" << argv[0] << "<port>";
		exit(1);
	}

	SOCKET serv_sock, clnt_sock;
	SOCKADDR_IN serv_addr, clnt_addr;
	int clnt_addr_len;
	char buf[MAXLEN] = "hello socket";

	WSADATA wsadata;
	WSAStartup(MAKEWORD(2, 2), &wsadata); // o:0   x:error code
	serv_sock = socket(AF_INET, SOCK_STREAM, 0);//o:0   x:INVALID_SOCKET

	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = PF_INET;
	serv_addr.sin_addr.s_addr = htonl(ADDR_ANY);
	serv_addr.sin_port = htons(atoi(argv[1]));

	if (SOCKET_ERROR == bind(serv_sock, (SOCKADDR *)&serv_addr, sizeof(serv_addr)))
	{

		showError("bind error");
	}

	if (SOCKET_ERROR == listen(serv_sock, 5))
	{
		showError("listen error");
	}

	clnt_addr_len = sizeof(clnt_addr);
	clnt_sock = accept(serv_sock, (SOCKADDR *)&clnt_addr, &clnt_addr_len);
	if (clnt_sock == INVALID_SOCKET)
	{
		showError("accept error");
	}

	char *pc = buf;
	int i = 0;
	while(i < strlen(buf))
	{
		send(clnt_sock, pc, 1, 0);
		printf("%c\t", *pc);
		pc++;
		i++;
	}

	//send(clnt_sock, buf, strlen(buf), 0);

	closesocket(clnt_sock);
	closesocket(serv_sock);

	WSACleanup();// o:0   x:SOCKET_ERROR
	system("pause");
	return 0;
}

