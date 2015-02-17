#include "Header.h"
int MasSize = 0;
int Value = 0;

char* FileName(int Num)
{
	char *FName[2] = { "Numbers1.txt", "Numbers2.txt" };
	return FName[Num];
}

int* ReadFile_A()
{
	errno_t err;
	int *ms = (int*)calloc(MAS_SIZE, sizeof(int));
	FILE *Read;
	char *fileName = "Numbers3.txt";
	switch (Value)
	{
	case 0:
		fileName = FileName(Value);
		break;
	case 1:
		fileName = FileName(Value);
		break;
	}
	err = fopen_s(&Read, fileName, "r");
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

char* ConvertIntToChar()
{
	int *mas;
	int size = 10;

	mas = (int*)calloc(MAS_SIZE, sizeof(int));
	mas = ReadFile_A();

	return Convert(mas, MasSize);
}

int main(int argc, char *argv[])
{
	HANDLE hSemaphore = CreateSemaphoreA(NULL,				// NULL - the handle cannot be inherited
										1,					// First status semaphore
										1,					// Maximum count for semaphore object
										SemName);			// IpName
	
	HANDLE hMapFile = CreateFileMappingA(					// указатель на спроецированную область памяти
		INVALID_HANDLE_VALUE,								// для выделения общей области памяти
		NULL,												// атрибут безопастности (по умолчанию) 
		PAGE_READWRITE,										// флаги защиты выделенной памяти
		0,													// макс. размер объекта 
		STR_SIZE,											// размер буфера  
		IpName);											// имя отраженного в памяти объекта
	
	char *lpBuff = static_cast<char *>(MapViewOfFile(hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, STR_SIZE));
	
	STARTUPINFO startupInfo;
	PROCESS_INFORMATION procInfo;
	memset(&procInfo, 0, sizeof(procInfo));
	memset(&startupInfo, 0, sizeof(startupInfo));
	startupInfo.cb = sizeof(startupInfo);					

	int result = CreateProcess(appName,						// File name
		NULL,												// Command line
		NULL,												// Process potection attr
		NULL,												// Thread potection attr
		FALSE,												// Handle inheritance 
		0,													// Creation flags
		NULL,												// Parent's environment block
		NULL,												// Parent's starting directory 
		&startupInfo,
		&procInfo);

	if (result == 0)
	{
		printf("Error: %d\n", GetLastError());
		system("pause");
		return 1;
	}

	char stop;
	do
	{
		WaitForSingleObject(hSemaphore, INFINITE);
		
		MasSize = 0;
		fflush(stdin);
		stop = _getch();
		Value = stop - '0';
		char *str = ConvertIntToChar();
		printf("    Mas = %30s\n", str);
		strcpy_s(lpBuff, STR_SIZE, str);
		
		ReleaseSemaphore(hSemaphore, 1, NULL);
	} while (stop != 'q');

	TerminateProcess(procInfo.hProcess,						// Handle to the process 
					 0);									// Exit code
	CloseHandle(hSemaphore);
	UnmapViewOfFile(lpBuff);
	CloseHandle(hMapFile);
	return 0;
}