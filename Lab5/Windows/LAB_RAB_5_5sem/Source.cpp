//D:\Microsoft Visual Studio TEMP\C\LAB_RAB_5_5sem\LAB_RAB_5_5sem

#include <stdio.h>
#include <string.h>
#include <windows.h>
#include <conio.h>
#include <fcntl.h>
#include <io.h>

#define MAX_LEN 8
#define PATH_COUNT 4

typedef DWORD(ReadFuncType)(CHAR *, DWORD, CHAR *, DWORD);
typedef void(WriteFuncType)(CHAR *, HANDLE);

LPCWSTR pathToLibrary = L"dDll.dll";

/*char* ReturnPath(int i)
{
	char str[PATH_COUNT][80] = { "D:\\Microsoft Visual Studio TEMP\\C\\LAB_RAB_5_5sem\\LAB_RAB_5_5sem\\out",
		"D:\\Microsoft Visual Studio TEMP\\C\\LAB_RAB_5_5sem\\LAB_RAB_5_5sem\\First.txt",
		"D:\\Microsoft Visual Studio TEMP\\C\\LAB_RAB_5_5sem\\LAB_RAB_5_5sem\\Second.txt",
		"D:\\Microsoft Visual Studio TEMP\\C\\LAB_RAB_5_5sem\\LAB_RAB_5_5sem\\Third.txt" };
	return str[i];
}*/
/*char* ReturnPath(int i)
{
	char str[PATH_COUNT][80] = { "out",
		"First.txt",
		"Second.txt",
		"Third.txt" };
	return str[i];
}*/

struct MyThreadArgs
{
	char				fName[MAX_LEN];
	HANDLE				*hWriteEventEnd;
	HANDLE				*hReadEventEnd;
	char				strData[MAX_LEN + 1];
	bool				bExit;
	WriteFuncType		*lpWriteFunc;
};

HINSTANCE gMyDll;

DWORD WINAPI WriterThread(void * args)
{
	MyThreadArgs *myArgs = static_cast<MyThreadArgs*>(args);
	if (myArgs == NULL)
		return 0;

	HANDLE hFile = CreateFileA(myArgs->fName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_FLAG_OVERLAPPED, NULL);
	if (hFile == 0)
		return 0;

	while (!myArgs->bExit)
	{
		WaitForSingleObject(myArgs->hReadEventEnd, INFINITE);
		puts("wRITE cOMPLITE");
		(*myArgs->lpWriteFunc)(myArgs->strData, hFile);
		SetEvent(myArgs->hWriteEventEnd);
	}

	return 0;
}

int main(int argc, char *argv[])
{
	if (argc <= 2){
		puts("Invalid args\nFirst arg -- output file\nElse args -- list of input files");
		return 1;
	}
	
	gMyDll = LoadLibrary(pathToLibrary);
	ReadFuncType *readFunc = reinterpret_cast<ReadFuncType *>(GetProcAddress(gMyDll, "MyRead"));

	HANDLE hWriteEventEnd = CreateEvent(NULL, FALSE, FALSE, NULL);
	HANDLE hReadEventEnd = CreateEvent(NULL, FALSE, FALSE, NULL);

	MyThreadArgs	args = {};
	args.hWriteEventEnd = &hWriteEventEnd;
	args.hReadEventEnd = &hReadEventEnd;
	args.strData[0] = 0;
	args.bExit = false;
	args.lpWriteFunc = reinterpret_cast<WriteFuncType *>(GetProcAddress(gMyDll, "MyWrite"));
	strcpy_s(args.fName, MAX_LEN, argv[1]);

	HANDLE hWriteThread = CreateThread(NULL, 0, &WriterThread, &args, 0, NULL);
	for (int i = 2; i < argc; i++)
	{
		DWORD newOffset = 0;
		do{
			newOffset = (*readFunc)(args.strData, MAX_LEN, argv[i], newOffset);
			if (newOffset == 0)
				break;

			SetEvent(&hReadEventEnd);
			WaitForSingleObject(&hWriteEventEnd, INFINITE);
			puts("rEAD cOMPLITE");
		} while (newOffset > 0);
	}
	puts("tHE eND");
	args.bExit = true;

	SetEvent(&hReadEventEnd);
	WaitForSingleObject(&hWriteThread, INFINITE);

	FreeLibrary(gMyDll);
	CloseHandle(&hWriteEventEnd);
	CloseHandle(&hReadEventEnd);
	CloseHandle(hWriteThread);

	return 0;
}



