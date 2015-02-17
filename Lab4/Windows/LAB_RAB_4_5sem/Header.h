#include <stdio.h>
#include <conio.h>
#include <string.h>
#include <windows.h>
#include <process.h>

#define COUNT 10
#define STR_LEN 200

struct MyThreadsArgs
{
	char				String[STR_LEN];
	CRITICAL_SECTION	*CriticalSection;
};