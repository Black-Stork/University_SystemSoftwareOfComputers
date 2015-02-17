#include <stdlib.h>
#include <set>
#include <list>
#include <iostream>
#include <Windows.h>
#include <conio.h>

#define MIN_BLOCK_SIZE 100
using namespace std;


// √раница. ≈сли IsStart == true, то это лева€ граница пам€ти под какую-то переменную(массив).
// ≈сли IsStart == false, то это права€ граница ( конец пам€ти по какую-то переменную\массив ).
struct Border
{
	Border(short* mem, bool isStart) : Mem(mem), IsStart(isStart) {}
	friend bool operator < (const Border &l, const Border &r){ return l.Mem < r.Mem; }
	short  *Mem;
	bool    IsStart;
};

struct Block
{
	Block(short *mem, size_t size) : Mem(mem), MemSize(size) {};
	~Block(){ free(Mem); }
	short          *Mem;
	size_t          MemSize;
	set< Border >   Borders;
};

list< Block* > gListOfBlocks;

void MarkAsOccupied(Block &block, short *start, size_t bytesNeed)
{
	block.Borders.insert(Border(start, true));
	block.Borders.insert(Border(start + bytesNeed, false));
}

void *FindFreeSpace(Block &block, size_t bytesNeed){

	if (block.Mem == nullptr)
	{
		block.MemSize = (bytesNeed < MIN_BLOCK_SIZE) ? MIN_BLOCK_SIZE : bytesNeed;
		block.Mem = (short *)malloc(block.MemSize);
		MarkAsOccupied(block, block.Mem, bytesNeed);
		return block.Mem;
	}

	if (block.Borders.size() == 2)
	{
		auto startBlock = block.Borders.begin();
		auto endBlock = block.Borders.begin(); ++endBlock;
		size_t freeSpace = block.MemSize - (endBlock->Mem - startBlock->Mem);
		if (freeSpace >= bytesNeed)
		{
			MarkAsOccupied(block, endBlock->Mem + 1, bytesNeed);
			return endBlock->Mem + 1;
		}
	}

	auto startFirstBlockIt = block.Borders.begin();
	auto endFirstBlock = startFirstBlockIt;
	size_t occupied = 0;
	while (true)
	{
		auto endFirstBlock = startFirstBlockIt; ++endFirstBlock;
		auto startSecondBlock = endFirstBlock; ++startSecondBlock;

		if (startSecondBlock == block.Borders.end())
			break;

		if (static_cast< size_t >(startSecondBlock->Mem - endFirstBlock->Mem) >= bytesNeed)
		{
			MarkAsOccupied(block, endFirstBlock->Mem + 1, bytesNeed);
			return endFirstBlock->Mem + 1;
		}

		startFirstBlockIt = startSecondBlock;
	}

	// ≈сли мы тут, то сработало условие startSecondBlock == block.Borders.end()
	auto endLastBlock = startFirstBlockIt; ++endLastBlock;
	if (static_cast< size_t >(block.Mem + block.MemSize - endLastBlock->Mem) >= bytesNeed)
	{
		MarkAsOccupied(block, endLastBlock->Mem + 1, bytesNeed);
		return endLastBlock->Mem + 1;
	}
	return nullptr;
}

void *Allocate(size_t bytesNeed)
{
	for each(auto currentBlock in gListOfBlocks)
	{
		auto resPtr = FindFreeSpace(*currentBlock, bytesNeed);
		if (resPtr != nullptr)
			return resPtr;
	}

	gListOfBlocks.push_back(new Block(nullptr, 0));
	auto last = gListOfBlocks.end(); --last;
	return FindFreeSpace(**last, bytesNeed);
}

void DisposeFromBlock(Block &block, void *mem)
{
	short *byteMem = static_cast<short *>(mem);
	Border borderForFind(byteMem, true);
	auto startMem = block.Borders.find(borderForFind);
	auto endMem = block.Borders.upper_bound(borderForFind);
	_ASSERT(startMem != block.Borders.end() && endMem != block.Borders.end());
	_ASSERT(startMem->IsStart || !endMem->IsStart);
	block.Borders.erase(startMem);
	block.Borders.erase(endMem);
	if (block.Borders.size() == 0)
	{		// ≈сли целый блок пуст, то освободим его блок пам€ти
		free(block.Mem);
		block.Mem = nullptr;
		block.MemSize = 0;
	}
}

void Dispose(void *mem)
{
	short *shortPtr = static_cast< short * >(mem);
	Border border(shortPtr, true);
	for each(auto currentBlock in gListOfBlocks)
	{
		if (currentBlock->Borders.find(border) != currentBlock->Borders.end())
		{
			DisposeFromBlock(*currentBlock, mem);
			return;
		}
	}
}

void *ReAllocate(void *oldMem, size_t newSize)
{
	Dispose(oldMem);
	return Allocate(newSize);
}

void PrintString(char* Dispose)
{
	for (int i = 0; i < strlen(Dispose); i++)
	{
		printf("%c", Dispose[i]);
		Sleep(20);
	}
}

HANDLE ghMutex;
HANDLE aThread[2];
HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);


int *intPtr = (int *)Allocate(sizeof(int));
long int *llPtr = (long int *)Allocate(sizeof(long int));

void AllocationThread()
{
	ghMutex = OpenMutexA(SYNCHRONIZE, false, "mutexos");
	
	for (int i = 0; i < 10; i++) 
	{
		WaitForSingleObject(ghMutex, INFINITE);
		//SetConsoleTextAttribute(hConsole, FOREGROUND_GREEN | FOREGROUND_INTENSITY);
		printf("\n[%d A] ", i);
		PrintString("Memory allocate is waiting..\n");

		llPtr = (long int *)Allocate(sizeof(long int));
		
		PrintString("\nSuccessfull allocation memory..\n");
		//SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY);

		ReleaseMutex(ghMutex);

		Sleep(100);
	}
}

void DisposeThread()
{
	ghMutex = OpenMutexA(SYNCHRONIZE, false, "mutexos");

	for (int i = 0; i < 10; i++)
	{
		WaitForSingleObject(ghMutex, INFINITE);
		//SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_INTENSITY);
		printf("\n[%d D] ", i);
		PrintString("Memory dispose is waiting..\n");

		Dispose(llPtr);

		PrintString("\nSuccessfull disposing memory..\n");
		//SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
		ReleaseMutex(ghMutex);

		Sleep(100);
	}
}

void PrintInformation(long int *mem)
{
	if (mem != NULL)
	{
		PrintString("\n\tINFORMATION:\n");
		cout << "\nMEMORY ADDRESS = " << mem << " | VALUE = ";
		cout << *mem << endl;
	}
}

int main()
{
	DWORD ThreadID;
	ghMutex = CreateMutexA(NULL, false, "mutexos");

	do
	{
		printf("\nPlease, input Number = ");
		scanf_s("%d", llPtr);

		aThread[0] = CreateThread(
			NULL,       // default security attributes
			0,          // default stack size
			(LPTHREAD_START_ROUTINE)DisposeThread,
			NULL,       // no thread function arguments
			0,          // default creation flags CREATE_SUSPENDED?
			&ThreadID); // receive thread identifier
		aThread[1] = CreateThread(
			NULL,       // default security attributes
			0,          // default stack size
			(LPTHREAD_START_ROUTINE)AllocationThread,
			NULL,       // no thread function arguments
			0,          // default creation flags CREATE_SUSPENDED?
			&ThreadID); // receive thread identifier

		do
		{
			WaitForSingleObject(ghMutex, INFINITE);
			PrintInformation(llPtr);
			ReleaseMutex(ghMutex);
			fflush(stdin);
		} while (_getch() != 27);

		WaitForSingleObject(ghMutex, INFINITE);
		PrintInformation(llPtr);
		ReleaseMutex(ghMutex);

		WaitForMultipleObjects(2, aThread, TRUE, INFINITE);
		CloseHandle(aThread[0]);
		CloseHandle(aThread[1]);
		CloseHandle(ghMutex);

		printf("\nTo repeat?\n");
		fflush(stdin);
	} while (_getch() != 27);
	system("pause");
	return 0;
}