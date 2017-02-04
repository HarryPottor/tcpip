#ifdef DEBUG


#include <stdio.h>
#include <windows.h>
#include <process.h>
#define Count 50


unsigned WINAPI ThreadFuncInc(void * arg);
unsigned WINAPI ThreadFuncDes(void * arg);

long long int sum = 0;
int main()
{
	
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
	if ((result = WaitForMultipleObjects(Count, Threads, FALSE, INFINITE)) == WAIT_FAILED)
	{
		puts("waitformultipleobjects error");
		return -1;
	}

	printf("sum = %lld\n", sum);
	system("pause");
	return 0;
}

unsigned WINAPI ThreadFuncInc(void * arg)
{
	for (size_t i = 0; i < 1000; i++)
	{
		sum += 1;
	}
}

unsigned WINAPI ThreadFuncDes(void * arg)
{
	for (size_t i = 0; i < 1000; i++)
	{
		sum -= 1;
	}
}
#endif // DEBUG