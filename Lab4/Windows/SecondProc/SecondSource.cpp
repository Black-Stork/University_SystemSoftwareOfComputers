#include "D:\\Microsoft Visual Studio TEMP\\C\\LAB_RAB_4_5sem\\LAB_RAB_4_5sem\\Header.h"

void main(int argc, char *argv[])
{

	MyThreadsArgs *lpArgs = static_cast<MyThreadsArgs *>(argv[0]);
	if (lpArgs == NULL)
		ExitThread(1);

	while (1)
	{
		EnterCriticalSection(lpArgs->CriticalSection);
		int i = 0;
		while (lpArgs->String[i] != 0)
		{
			printf("%c", lpArgs->String[i++]);
			Sleep(10);
		}
		puts("");
		LeaveCriticalSection(lpArgs->CriticalSection);
		Sleep(25);
	}
	ExitThread(0);
}