#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>
#include <conio.h>
#include <tchar.h>
#define MAX_LEN 50
int MasSize = 0;

int* ReadFile_A()
{
	errno_t err;
	int *ms = (int*)calloc(MAX_LEN, sizeof(int));
	FILE *Read;
	err = fopen_s(&Read, "Numbers1.txt", "r");
	while (!feof(Read))
	{
		fscanf_s(Read, "%d", &ms[MasSize]);
		MasSize++;
	}
	return ms;
}

char* Convert(int *ms, int size)
{
	char* temp = (char*)calloc(1, sizeof(char));
	char* str = (char*)calloc(50, sizeof(char));
	str[0] = 0;

	for (int i = 0; i < size; i++)
	{
		_itoa_s(ms[i], temp, 3, 10);
		strcat_s(str, 50, temp);
		strcat_s(str, 50, " ");
	}
	str[strlen(str) + 1] = 0;
	return str;
}

void Print(int *ms, int size)
{
	HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hStdout, FOREGROUND_GREEN | FOREGROUND_INTENSITY);
	char *ms2 = (char*)calloc(100, sizeof(char));

	printf("\n");
	ms2 = Convert(ms, size);
	puts(ms2);
}

int* Sort(int *ms, int size)
{
	int m;
	for (int i = 0; i < size - 1; i++)
	for (int j = i + 1; j < size; j++)
	{
		if (ms[i] > ms[j])
		{
			m = ms[i];
			ms[i] = ms[j];
			ms[j] = m;
		}
		
	}
	Print(ms, size);
	return ms;
}

void FirstProc()
{
	int *ms2;
	int size = 10;

	ms2 = (int*)calloc(MAX_LEN, sizeof(int));

	/*int ms[10] = { 8, 9, 2, 3, 5, 1, 4, 6, 0, 7 };
	for (int i = 0; i < size; i++)
		ms2[i] = ms[i];*/
	ms2 = ReadFile_A();

	ms2 = Sort(ms2, MasSize);

	_getch();
}

int main()
{
	//ReadFile_A();

	STARTUPINFO startupInfo;
	PROCESS_INFORMATION procInfo;
	memset(&procInfo, 0, sizeof(procInfo));
	memset(&startupInfo, 0, sizeof(startupInfo));
	TCHAR *appName = L"D:\\Microsoft Visual Studio TEMP\\C\\LAB_RAB_1_5sem\\Debug\\SecondProc.exe";

	startupInfo.cb = sizeof(startupInfo); //    The size of the structure, in bytes.

	int result = CreateProcess(appName,   //file name
			NULL,        // Command line
			NULL,           // Process potection attr
			NULL,           // Thread potection attr
			FALSE,          //  handle inheritance 
			0,              //  creation flags
			NULL,           //  parent's environment block
			NULL,           //  parent's starting directory 
			&startupInfo,
			&procInfo);
		//Sleep(2000);
		FirstProc();

	if (result == 0)
	{
		printf("error %d\n", GetLastError());
		system("pause");
		return 1;
	}
	else
	{
		WaitForSingleObject(procInfo.hProcess, INFINITE);
		if (!CloseHandle(procInfo.hProcess))
			printf("error %d\n", GetLastError());

		if (!CloseHandle(procInfo.hThread))
			printf("error %d\n", GetLastError());
	}
	system("pause");
	return 0;
}