#include <stdio.h>
#include <winsock2.h>
#include <iostream>
#include <windows.h>
int main()
{
	WSAData wsadata;
	SOCKADDR_IN addr;
	LPWSTR str = L"1.2.3.4:9090";
	WCHAR netstr[50];
	WSAStartup(MAKEWORD(2, 2), &wsadata);
	int size = sizeof(addr);
	if (WSAStringToAddress(str, AF_INET, NULL, (SOCKADDR*)&addr, &size) == SOCKET_ERROR)
	{
		puts("error");
	}
	DWORD size2 = sizeof(netstr); 
	WSAAddressToString((SOCKADDR*)&addr, sizeof(addr), NULL, netstr, &size2);

	/*
	int i = 0;
	while (netstr[i] != '\0')
	{
		printf("%lc", netstr[i]);
		i++;
	}
	*/

	std::wcout << netstr << std::endl;
	WSACleanup();
	system("pause");
	return 0;
}