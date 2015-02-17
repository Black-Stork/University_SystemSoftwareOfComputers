#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>
#include <conio.h>
#include <tchar.h>
#include <string.h>
#include <process.h>
#define COUNT 10

LPWSTR ReturnStr(int Num)
{
	LPWSTR str[COUNT] = {L"0000", L"11111", 
						 L"222222", L"3333333", 
						 L"44444444", L"555555555", 
						 L"6666666666", L"77777777777", 
						 L"888888888888", L"9999999999999"};
	
	return str[Num];
}

void CloseAllProc(PROCESS_INFORMATION *procInfo, int lastProc)
{
	for (lastProc; lastProc >= 0; --lastProc)
		TerminateProcess(procInfo[lastProc].hProcess, 0);
}

int main()
{
	STARTUPINFO startupInfo;
	PROCESS_INFORMATION procInfo[COUNT];
	memset(&procInfo, 0, COUNT * sizeof(PROCESS_INFORMATION));
	memset(&startupInfo, 0, sizeof(STARTUPINFO));
	LPCWSTR appName = L"D:\\Microsoft Visual Studio TEMP\\C\\LAB_RAB_2_5sem\\Debug\\SecondProc.exe";
	int lastProc = -1;

	HANDLE hPermissionEvent = CreateEventA(NULL,	//	PSECURITY_ATTRIBUTES 
		FALSE,										//	fManualReset, true for manual
		FALSE,										//	fInitialState, true for signal and false for notsignal
		"First Event");								//	EventName

	if (hPermissionEvent == NULL)
	{
		puts(" hPermissionEvent == NULL ");
		return GetLastError();
	}

	startupInfo.cb = sizeof(startupInfo); //    The size of the structure, in bytes.

	while(1)
	{
		char simbol = _getch();
		if (simbol == '+' && (lastProc + 1) >= COUNT)
			continue;

		if (simbol == '+')
		{
			/*STARTUPINFO startupInfo = { 0 };
			startupInfo.cb = sizeof(STARTUPINFO);*/

			lastProc++;
			/*if (lastProc >= COUNT)
				break;*/

			int result = CreateProcess(appName, //file name
				ReturnStr(lastProc),			// Command line
				NULL,							// Process potection attr
				NULL,							// Thread potection attr
				FALSE,							//  handle inheritance 
				0,								//  creation flags
				NULL,							//  parent's environment block
				NULL,							//  parent's starting directory 
				&startupInfo,
				&procInfo[lastProc]);

			if (result == 0)
			{
				printf("Error %d\n", GetLastError());
				system("pause");
				return 1;
			}

			SetEvent(hPermissionEvent);
		}

		if (simbol == '-')
		{
			if (lastProc < 0)
				lastProc++;

			
			TerminateProcess(procInfo[lastProc--].hProcess, 0);
			SetEvent(hPermissionEvent);
		}
		if (simbol == 'q')
		{
			CloseAllProc(procInfo, lastProc);
			CloseHandle(hPermissionEvent);
			return 0;
		}
		if (simbol == 'c')
			system("cls");
	}

	system("pause");
	return 0;
}