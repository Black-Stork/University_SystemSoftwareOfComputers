//#include <stdlib.h>
//#include <set>
//#include <list>
//#include <iostream>
//#include <Windows.h>
//#include <conio.h>
//
//#define MIN_BLOCK_SIZE 100
//using namespace std;
//
//// Граница. Если IsStart == true, то это левая граница памяти под какую-то переменную(массив).
//// Если IsStart == false, то это правая граница ( конец памяти по какую-то переменную\массив ).
//struct Border
//{
//	Border(short* mem, bool isStart) : Mem(mem), IsStart(isStart) {}
//	friend bool operator < (const Border &l, const Border &r){ return l.Mem < r.Mem; }
//	short  *Mem;
//	bool    IsStart;
//};
//
//struct Block
//{
//	Block(short *mem, size_t size) : Mem(mem), MemSize(size) {};
//	~Block(){ free(Mem); }
//	short          *Mem;
//	size_t          MemSize;
//	set< Border >   Borders;
//};
//
//list< Block* > gListOfBlocks;
//
//void MarkAsOccupied(Block &block, short *start, size_t bytesNeed)
//{
//	block.Borders.insert(Border(start, true));
//	block.Borders.insert(Border(start + bytesNeed, false));
//}
//
//void *FindFreeSpace(Block &block, size_t bytesNeed){
//	
//	if (block.Mem == nullptr)
//	{
//		block.MemSize = (bytesNeed < MIN_BLOCK_SIZE) ? MIN_BLOCK_SIZE : bytesNeed;
//		block.Mem = (short *)malloc(block.MemSize);
//		MarkAsOccupied(block, block.Mem, bytesNeed);
//		return block.Mem;
//	}
//
//	if (block.Borders.size() == 2)
//	{
//		auto startBlock = block.Borders.begin();
//		auto endBlock = block.Borders.begin(); ++endBlock;
//		size_t freeSpace = block.MemSize - (endBlock->Mem - startBlock->Mem);
//		if (freeSpace >= bytesNeed)
//		{
//			MarkAsOccupied(block, endBlock->Mem + 1, bytesNeed);
//			return endBlock->Mem + 1;
//		}
//	}
//
//	auto startFirstBlockIt = block.Borders.begin();
//	auto endFirstBlock = startFirstBlockIt;
//	size_t occupied = 0;
//	while (true)
//	{
//		auto endFirstBlock = startFirstBlockIt; ++endFirstBlock;
//		auto startSecondBlock = endFirstBlock; ++startSecondBlock;
//
//		if (startSecondBlock == block.Borders.end())
//			break;
//
//		if (static_cast< size_t >(startSecondBlock->Mem - endFirstBlock->Mem) >= bytesNeed)
//		{
//			MarkAsOccupied(block, endFirstBlock->Mem + 1, bytesNeed);
//			return endFirstBlock->Mem + 1;
//		}
//
//		startFirstBlockIt = startSecondBlock;
//	}
//
//	// Если мы тут, то сработало условие startSecondBlock == block.Borders.end()
//	auto endLastBlock = startFirstBlockIt; ++endLastBlock;
//	if (static_cast< size_t >(block.Mem + block.MemSize - endLastBlock->Mem) >= bytesNeed)
//	{
//		MarkAsOccupied(block, endLastBlock->Mem + 1, bytesNeed);
//		return endLastBlock->Mem + 1;
//	}
//	return nullptr;
//}
//
//void *Allocate(size_t bytesNeed)
//{
//	for each(auto currentBlock in gListOfBlocks)
//	{
//		auto resPtr = FindFreeSpace(*currentBlock, bytesNeed);
//		if (resPtr != nullptr)
//			return resPtr;
//	}
//
//	gListOfBlocks.push_back(new Block(nullptr, 0));
//	auto last = gListOfBlocks.end(); --last;
//	return FindFreeSpace(**last, bytesNeed);
//}
//
//void DisposeFromBlock(Block &block, void *mem)
//{
//	short *byteMem = static_cast<short *>(mem);
//	Border borderForFind(byteMem, true);
//	auto startMem = block.Borders.find(borderForFind);
//	auto endMem = block.Borders.upper_bound(borderForFind);
//	_ASSERT(startMem != block.Borders.end() && endMem != block.Borders.end());
//	_ASSERT(startMem->IsStart || !endMem->IsStart);
//	block.Borders.erase(startMem);
//	block.Borders.erase(endMem);
//	if (block.Borders.size() == 0)
//	{		// Если целый блок пуст, то освободим его блок памяти
//		free(block.Mem);
//		block.Mem = nullptr;
//		block.MemSize = 0;
//	}
//}
//
//void Dispose(void *mem)
//{
//	short *shortPtr = static_cast< short * >(mem);
//	Border border(shortPtr, true);
//	for each(auto currentBlock in gListOfBlocks)
//	{
//		if (currentBlock->Borders.find(border) != currentBlock->Borders.end())
//		{
//			DisposeFromBlock(*currentBlock, mem);
//			return;
//		}
//	}
//}
//
//void *ReAllocate(void *oldMem, size_t newSize)
//{
//	Dispose(oldMem);
//	return Allocate(newSize);
//}
//
//void PrintString(char* Dispose)
//{
//	for (int i = 0; i < strlen(Dispose); i++)
//	{
//		printf("%c", Dispose[i]);
//		Sleep(80);
//	}
//}
//
//int main()
//{
//	int *intPtr = (int *)Allocate(sizeof(int));
//	long int *llPtr = (long int *)Allocate(sizeof(long int));
//	
//	do
//	{
//		printf("\nPlease, input Number = ");
//		scanf_s("%d", llPtr);
//		//*llPtr = 1234567890;
//
//		cout << "\nMEMORY ADDRESS = " << llPtr << " | VALUE = ";						// Адрес
//		Dispose(llPtr);																	// Освобождаем память
//		cout << *llPtr << endl;															// На самом деле память не освобождена, а помечена как свободная.
//
//		PrintString("\nDispose...");
//
//		llPtr = nullptr;
//		PrintString("\nMemory allocation...");
//		llPtr = (long int *)Allocate(sizeof(long int));
//		PrintString("\nAfter memory allocation...");
//		cout << "\n\nMEMORY ADDRESS = " << llPtr << " | VALUE = " << *llPtr << endl;	// Те же данные и адрес
//
//		*llPtr = 99999999;
//		PrintString("\nChange the value: ");
//		cout << *llPtr;
//		PrintString("\nAfter changing...");
//		cout << "\n\nMEMORY ADDRESS = " << llPtr << " | VALUE = " << *llPtr << endl;	// Те же данные и адрес
//	
//		PrintString("\nPress Esc, for exit...\n");
//	} while (_getch() != 27);
//
//	Dispose(llPtr);
//	Dispose(intPtr);
//
//	return 0;
//}