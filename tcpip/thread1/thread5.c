#include <stdio.h>
#include <Windows.h>
#include <process.h>


unsigned WINAPI GetACount(void *);
unsigned WINAPI GetNotACount(void *);

HANDLE hEvent;

int main()
{

	hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

	HANDLE threads[2];

	char buf[100];
	fputs("Input a string:", stdout);
	scanf("%s", buf);


	threads[0] = (HANDLE)_beginthreadex(NULL, 0, GetACount, (void*)buf, 0, NULL);
	threads[1] = (HANDLE)_beginthreadex(NULL, 0, GetNotACount, (void*)buf, 0, NULL);
	SetEvent(hEvent);

	WaitForMultipleObjects(2, threads, true, INFINITE);
	ResetEvent(hEvent);

	CloseHandle(hEvent);
	system("pause");
	return 0;
}

unsigned WINAPI GetACount(void *arg)
{
	char * str = (char*)arg;
	int cnt = 0;
	WaitForSingleObject(hEvent, INFINITE);
	while (*str)
	{
		if (*str == 'A')
		{
			cnt++;
		}
		str++;
	}
	printf("number of A: %d\n", cnt);

	return 0;
}

unsigned WINAPI GetNotACount(void *arg)
{
	char * str = (char*)arg;
	int cnt = 0;
	WaitForSingleObject(hEvent, INFINITE);
	while (*str)
	{
		if (*str != 'A')
		{
			cnt++;
		}
		str++;
	}
	printf("number of not A: %d\n", cnt);

	return 0;
}
