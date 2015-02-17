#include <stdio.h>
#include <conio.h>
#include <string.h>
#include <windows.h>
#include <process.h>

#define MAX_PROC_COUNT 10
#define MAX_STR_SIZE 200

struct MyThreadsArgs{
	char				String[MAX_STR_SIZE];
	CRITICAL_SECTION*	CriticalSection;
};

DWORD WINAPI childThread(LPVOID pvParam)
{
	printf("Hello! I am a Printer\n");

	MyThreadsArgs *lpArgs = static_cast<MyThreadsArgs *>(pvParam);
	if (lpArgs == NULL){
		ExitThread(EXIT_FAILURE);
	}
	while (true){
		EnterCriticalSection(lpArgs->CriticalSection);
		int i = 0;
		while (lpArgs->String[i] != 0){
			printf("%c", lpArgs->String[i++]);
			Sleep(10);
		}
		puts("");
		LeaveCriticalSection(lpArgs->CriticalSection);
		Sleep(25);
	}
	ExitThread(EXIT_SUCCESS);
}

void CloseAllThreads(HANDLE *hThreads, int lastProc)
{
	for (; lastProc >= 0; --lastProc){
		TerminateThread(hThreads[lastProc], 0);
	}
}

int main(int , char **)
{
	printf("Hello! I am process 1\n");
	
	CRITICAL_SECTION criticalSection;
	InitializeCriticalSection(&criticalSection);

	HANDLE			hThreads[MAX_PROC_COUNT];
	MyThreadsArgs	threadArgs[MAX_PROC_COUNT];
	ZeroMemory(hThreads, MAX_PROC_COUNT * sizeof(HANDLE));
	ZeroMemory(threadArgs, MAX_PROC_COUNT * sizeof(MyThreadsArgs));
	
	int lastProc = -1;
	while (true){
		switch (_getch()){
		case '+':
		{
					++lastProc;
					if (lastProc >= MAX_PROC_COUNT){
						puts("OOOO NOOOO! SO MANY PROCESSES!!!");
						break;
					}
					char myStr[MAX_STR_SIZE];
					
					EnterCriticalSection(&criticalSection);
					puts("Print string");
					fflush(stdin);
					gets_s(myStr, MAX_STR_SIZE);
					LeaveCriticalSection(&criticalSection);

					threadArgs[lastProc].CriticalSection = &criticalSection;
					strcpy_s(threadArgs[lastProc].String, MAX_STR_SIZE, myStr);

					hThreads[lastProc] = CreateThread(NULL, 0, &childThread, &threadArgs[lastProc], 0, NULL);
					if (hThreads[lastProc] == NULL){
						--lastProc;
						puts("CreateThread error");
					}
					break;
		}
		case '-':
		{
					EnterCriticalSection(&criticalSection);
					if (lastProc < 0){
						puts("NOOOOOO SO LESS PROCESSES");
					}
					else{
						TerminateThread(hThreads[lastProc--], 0);
					}
					LeaveCriticalSection(&criticalSection);
					break;
		}
		case 'q':
		{
					CloseAllThreads(hThreads, lastProc);
					DeleteCriticalSection(&criticalSection);
					return EXIT_SUCCESS;
		}
		}
	}
	return EXIT_SUCCESS;
}
