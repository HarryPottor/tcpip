#ifdef DEBUG



#include <stdio.h>
#include <windows.h>
#include <process.h>

unsigned WINAPI ThreadFunc(void * arg);

int main()
{
	HANDLE hThread;
	int param = 5;
	hThread = (HANDLE)_beginthreadex(NULL, 0, ThreadFunc, (void*)&param, 0,NULL);
	if (0 == hThread)
	{
		puts("_beginthread error()");
		return -1;
	}

	DWORD wr;
	if ((wr = WaitForSingleObject(hThread, 3000)) == WAIT_FAILED)
	{
		puts("waitforsingleobject error");
		return -1;
	}
	printf("wait result: %s\n", (wr == WAIT_OBJECT_0) ? "signaled" : "time out");

	puts("end of main");
	system("pause");
	return 0;
}

unsigned WINAPI ThreadFunc(void * arg)
{
	int param = *((int *)arg);
	for (int i = 0; i < param; i++)
	{
		Sleep(1000);
		puts("running thread");
	}

	return 0;
}

#endif // DEBUG