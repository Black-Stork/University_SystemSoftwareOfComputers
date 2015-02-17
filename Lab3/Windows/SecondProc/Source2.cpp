#include "D:\\Microsoft Visual Studio TEMP\\C\LAB_RAB_3_5sem\\LAB_RAB_3_5sem\\Header.h"
int MasSize = 0;

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
	return ms;
}

char* ConvertIntToChar(int *ms)
{
	char* temp = (char*)calloc(1, sizeof(char));
	char* str = (char*)calloc(50, sizeof(char));
	str[0] = 0;

	for (int i = 0; i < MasSize; i++)
	{
		_itoa_s(ms[i], temp, 3, 10);
		strcat_s(str, 50, temp);
		strcat_s(str, 50, " ");
	}
	str[strlen(str) + 1] = 0;

	return str;
}

int Power(int Input, int Level)
{
	int Num = 1;
	if (Level == 0)
		return 1;
	for (int i = 0; i < Level; i++)
		Num *= Input;
	return Num;
}

int* ConvertCharToInt(char *str)
{
	int *mas = (int*)calloc(MAS_SIZE, sizeof(int));
	int i = strlen(str) - 1;

	while (str[i] != NULL)
	{
		int Num = 0, level = 0;
		if (str[i] == ' ')
		{
			i--;
			continue;
		}
		while (isdigit(str[i]))
		{
			int ch = str[i] - '0';
			Num += ch * Power(10, level);
			i--; level++;
		}
		mas[MasSize] = Num;
		MasSize++; i--;
	}
	return mas;
}

char* Print(char *str)
{
	int *Mas = (int *)calloc(MAS_SIZE, sizeof(int));

	Mas = ConvertCharToInt(str);
	Mas = Sort(Mas, MasSize);

	return ConvertIntToChar(Mas);
}

int main(int argc, char *argv[])
{
	HANDLE hSemaphore = OpenSemaphoreA(SEMAPHORE_ALL_ACCESS, FALSE, SemName);
	
	HANDLE hMapFile = OpenFileMappingA(							// указатель на спроецированную область памяти
		FILE_MAP_ALL_ACCESS,									// доступ к чтению/записи
		FALSE,													// имя не наследуется
		IpName);												// имя "проецируемого " объекта 
	
	char *lpBuff = static_cast<char *>(MapViewOfFile(hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, STR_SIZE));

	while (1)
	{
		MasSize = 0;
		WaitForSingleObject(hSemaphore, INFINITE);
		printf("SortMas = %30s\n\n", Print(lpBuff));
		ReleaseSemaphore(hSemaphore, 1, NULL);
	}

	CloseHandle(hSemaphore);
	UnmapViewOfFile(lpBuff);
	CloseHandle(hMapFile);
	return 0;
}