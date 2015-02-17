// library.cpp : Defines the exported functions for the DLL application.
#include "stdafx.h"
#include "stdio.h"
#include "string.h"

#ifdef __cplusplus 
extern "C" {
#endif

	__declspec(dllexport) DWORD MyRead(CHAR *lpBuf, DWORD bufSize, CHAR *lpFileName, DWORD offset)
	{
		if ( lpBuf == NULL )
			return 0;

		HANDLE hFile = CreateFileA(lpFileName, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, NULL);

		OVERLAPPED	overlapped = {};
					overlapped.hEvent = CreateEventA(NULL, FALSE, FALSE, NULL);
					overlapped.Offset = offset;
		DWORD		byteReaded;

		ReadFile(hFile, lpBuf, bufSize, &byteReaded, &overlapped);
		WaitForSingleObject(overlapped.hEvent, INFINITE);

		BOOL res = GetOverlappedResult(hFile, &overlapped, &byteReaded, FALSE);
		DWORD newOffset = byteReaded + offset;
		lpBuf[ byteReaded ] = 0;
		if ( !res && GetLastError() == ERROR_HANDLE_EOF ){		// Был достигнут конец файла
			newOffset = 0;
		}

		CloseHandle(overlapped.hEvent);
		CloseHandle(hFile);
		return newOffset;
	}

	__declspec(dllexport) void MyWrite(CHAR *lpBuf, HANDLE hFile)
	{
		if ( lpBuf == NULL )
			return;

		OVERLAPPED	overlapped = {};
					overlapped.hEvent = CreateEventA(NULL, FALSE, FALSE, NULL);
					overlapped.Offset = 
					overlapped.OffsetHigh = 0xFFFFFFFF;
		DWORD		byteWrited;

		WriteFile(hFile, lpBuf, strlen(lpBuf), &byteWrited, &overlapped);
		WaitForSingleObject(overlapped.hEvent, INFINITE);
		
		CloseHandle(overlapped.hEvent);
	}

#ifdef __cplusplus
}
#endif