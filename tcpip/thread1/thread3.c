#ifdef DEBUG


#include <stdio.h>
#include <windows.h>
#include <process.h>
#define Count 50


unsigned WINAPI ThreadFuncInc(void * arg);
unsigned WINAPI ThreadFuncDes(void * arg);

long long int sum = 0;

HANDLE hMutex;
int main()
{
	hMutex = CreateMutex(NULL, FALSE, NULL);

	HANDLE Threads[Count];
	for (int i = 0; i < Count; i++)
	{
		if (i % 2 == 1)
		{
			Threads[i] = (HANDLE)_beginthreadex(NULL, 0, ThreadFuncInc, NULL, 0, NULL);
		}
		else
		{
			Threads[i] = (HANDLE)_beginthreadex(NULL, 0, ThreadFuncDes, NULL, 0, NULL);
		}
	}

	DWORD result;
	if ((result = WaitForMultipleObjects(Count, Threads, TRUE, INFINITE)) == WAIT_FAILED)
	{
		puts("waitformultipleobjects error");
		return -1;
	}
	printf("sum = %lld\n", sum);
	
	CloseHandle(hMutex);
	system("pause");
	return 0;
}

unsigned WINAPI ThreadFuncInc(void * arg)
{
	WaitForSingleObject(hMutex, INFINITE);
	for (size_t i = 0; i < 1000; i++)
	{
		sum += 1;
	}
	ReleaseMutex(hMutex);
	return 0;
}

unsigned WINAPI ThreadFuncDes(void * arg)
{
	WaitForSingleObject(hMutex, INFINITE);
	for (size_t i = 0; i < 1000; i++)
	{
		sum -= 1;
	}
	ReleaseMutex(hMutex);
	return 0;
}
#endif // DEBUG