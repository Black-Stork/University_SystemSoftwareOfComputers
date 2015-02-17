#include "Header.h"

char* ReturnStr(int Num)
{
	char* str[COUNT] = { "0", "11",
		"222", "3333",
		"44444", "555555",
		"6666666", "77777777",
		"888888888", "9999999999" };

	return str[Num];
}

DWORD WINAPI childThread(LPVOID pvParam)
{
	MyThreadsArgs *lpArgs = static_cast<MyThreadsArgs *>(pvParam);
	if (lpArgs == NULL)
		ExitThread(1);

	while (1)
	{
		Sleep(500);
		
		EnterCriticalSection(lpArgs->CriticalSection);
		
		int Len = strlen(lpArgs->String);
		int Value = lpArgs->String[0] - '0' + 1;
		printf("\n\t");
		for (int i = 0; i < Len; i++)
		{
			printf("%c", lpArgs->String[i]);
			Sleep(10 * Value);
		}
		LeaveCriticalSection(lpArgs->CriticalSection);
		
		Sleep(25 * Value);
	}
	ExitThread(0);
}

void CloseAllThreads(HANDLE *hThreads, int lastProc)
{
	for (lastProc; lastProc >= 0; --lastProc)
		TerminateThread(hThreads[lastProc], 0);
}

int main(int, char **)
{
	CRITICAL_SECTION criticalSection;
	InitializeCriticalSection(&criticalSection);

	HANDLE			hThreads[COUNT];
	MyThreadsArgs	threadArgs[COUNT];
	memset(&hThreads, 0, COUNT * sizeof(HANDLE));
	memset(&threadArgs, 0, COUNT * sizeof(MyThreadsArgs));
	int lastProc = -1;

	while (1)
	{
		char simbol = _getch();
		if (simbol == '+' && (lastProc + 1) >= COUNT)
			continue;

		if (simbol == '+')
		{
			lastProc++;

			threadArgs[lastProc].CriticalSection = &criticalSection;
			strcpy_s(threadArgs[lastProc].String, STR_LEN, ReturnStr(lastProc));

			hThreads[lastProc] = CreateThread(NULL, 0, &childThread, &threadArgs[lastProc], 0, NULL);

			if (hThreads[lastProc] == NULL)
				lastProc--;
		}

		if (simbol == '-')
		{
			EnterCriticalSection(&criticalSection);
			
			if (lastProc < 0)
				lastProc++;
			else
				TerminateThread(hThreads[lastProc--], 0);
			
			LeaveCriticalSection(&criticalSection);
		}
		if (simbol == 'q')
		{
			CloseAllThreads(hThreads, lastProc);
			DeleteCriticalSection(&criticalSection);
			break;
		}

	}
	return 0;
}

