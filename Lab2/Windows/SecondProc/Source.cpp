#include <stdio.h>
#include <string.h>
#include <windows.h>
#include <process.h>

int main(int argc, char *argv[])
{
	HANDLE hPermissionEvent = OpenEventA(EVENT_ALL_ACCESS,	//	DesiredAccess 
		FALSE,												//	InheritHandel
		"First Event");										//	EventName

	if (hPermissionEvent == NULL)
		return GetLastError();

	while (true)
	{		
		Sleep(500);
		WaitForSingleObject(hPermissionEvent, INFINITE);
		//printf("\n\t%s", argv[0]);
		
		int len = strlen(argv[0]);
		printf("\n\t");
		for (int i = 0; i < len; i++)
			printf("%c", argv[0][i]);
		
		SetEvent(hPermissionEvent);
	}

	CloseHandle(hPermissionEvent);

	return 0;
}