#ifdef DEBUG


#include <stdio.h>
#include <windows.h>
#include <process.h>
#define Count 50


unsigned WINAPI InputValue(void * arg);
unsigned WINAPI AddValue(void * arg);

long long int sum = 0;

HANDLE hSem[2];
int Value;
int main()
{
	hSem[0] = CreateSemaphore(NULL, 1, 1, NULL);
	hSem[1] = CreateSemaphore(NULL, 0, 1, NULL);

	HANDLE hThreads[2];
	int inputCount;
	fputs("Input counts :", stdout);
	scanf("%d", &inputCount);

	hThreads[0] = (HANDLE)_beginthreadex(NULL, 0, InputValue, (void*)&inputCount, 0, NULL);
	hThreads[1] = (HANDLE)_beginthreadex(NULL, 0, AddValue, (void*)&inputCount, 0, NULL);

	DWORD result;
	if ((result = WaitForMultipleObjects(2, hThreads, TRUE, INFINITE)) == WAIT_FAILED)
	{
		puts("waitformultipleobjects error");
		return -1;
	}

	printf("sum = %lld \n", sum);
	CloseHandle(hSem[0]);
	CloseHandle(hSem[1]);

	system("pause");
	return 0;
}

unsigned WINAPI InputValue(void * arg)
{
	int counts = *((int*)arg);
	
	for (int i = 0; i < counts; i++)
	{
		WaitForSingleObject(hSem[0], INFINITE);
		printf("Input %dth value: ", i + 1);
		scanf("%d", &Value);
		ReleaseSemaphore(hSem[1], 1, NULL);
	}
	

	return 0;
}

unsigned WINAPI AddValue(void * arg)
{
	int counts = *((int*)arg);

	for (int i = 0; i < counts; i++)
	{
		WaitForSingleObject(hSem[1], INFINITE);
		sum += Value;
		ReleaseSemaphore(hSem[0], 1, NULL);
	}
	return 0;
}
#endif // !DEBUG