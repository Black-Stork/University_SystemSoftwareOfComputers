////#include <SDKDDKVer.h>
//#include <tchar.h>
//#include <stdio.h>
//#include <conio.h>
//
//#define MEM_SIZE 24
//#define BLOCKS_COUNT (MEM_SIZE / 8)
//#define MARK_FREE 0
//#define MARK_ALLOC 1
//
//
//struct MemBlock
//{
//	void* start;
//	unsigned long lenght;
//};
//
//static char internalMemory[2][MEM_SIZE];
//
//static int BlocksCount;
//static MemBlock Blocks[BLOCKS_COUNT];
//
//int CheckForRealloc(void* ptr, unsigned newSize)
//{
//	unsigned size = newSize;
//	int k = ((char*)(ptr)-(char*)(internalMemory[0])) / sizeof(char);
//	while (internalMemory[1][k] == (char)MARK_ALLOC)
//	{
//		k++;
//		size--;
//	}
//	for (int i = 0; size>0; i++)
//	{
//		size--;
//		if (internalMemory[1][k + i] == (char)MARK_ALLOC)
//			return -1;
//	}
//
//	return 1;
//}
//
//void MarkBlock(MemBlock b, int mode)
//{
//	int k = ((char*)(b.start) - (char*)(internalMemory[0])) / sizeof(char);
//	for (int i = 0; i < b.lenght; i++)
//		internalMemory[1][k + i] = (char)mode;
//}
//
//int PasteBlock(MemBlock b)
//{
//	int i = 0;
//	if (BlocksCount != 0)
//	while (Blocks[i].start < b.start && Blocks[i].start != NULL)
//		i++;
//	BlocksCount++;
//	if (BlocksCount == BLOCKS_COUNT)
//	{
//		BlocksCount--;
//		return -1;
//	}
//	int k = BlocksCount - 1;
//	while (k>i)
//		Blocks[k--] = Blocks[i];
//	Blocks[i] = b;
//	return 1;
//}
//
//void* FindMem(unsigned size)
//{
//	if (size > MEM_SIZE)
//		return NULL;
//	for (int i = 0; i < MEM_SIZE - size; i++)
//	{
//		int good = 1;
//		for (int k = 0; k < size; k++)
//		if (internalMemory[1][i + k] == (char)1)
//		{
//			good = 0;
//			break;
//		}
//		if (good == 1)
//			return (void*)&internalMemory[0][i];
//	}
//	return NULL;
//}
//
//int DeleteBlock(void* start)
//{
//	int a = 0, b = BlocksCount - 1;
//	int c;
//	while (true)
//	{
//		c = (a + b) / 2;
//		if (Blocks[c].start == start)
//			break;
//		if (Blocks[c].start < start)
//		{
//			if (a == c)
//				return -1;
//			a = c + 1;
//		}
//		else
//		{
//			if (b == c)
//				return -1;
//			b = c - 1;
//		}
//	}
//	MarkBlock(Blocks[c], MARK_FREE);
//	int i = --BlocksCount;
//	while (c < i)
//	{
//		Blocks[c] = Blocks[c + 1];
//		c++;
//	}
//	return 1;
//}
//
//void* AllocMem(unsigned size)
//{
//	void* ptr = FindMem(size);
//	if (ptr == NULL)
//		return NULL;
//	MemBlock b;
//	b.start = ptr;
//	b.lenght = size;
//	PasteBlock(b);
//	MarkBlock(b, MARK_ALLOC);
//	return ptr;
//}
//
//int free(void* ptr)
//{
//	if (DeleteBlock(ptr) == -1)
//		return -1;
//	return 1;
//}
//
//void* realloc(void* ptr, unsigned newSize)
//{
//	int a = 0, b = BlocksCount - 1;
//	int c;
//	while (true)
//	{
//		c = (a + b) / 2;
//		if (Blocks[c].start == ptr)
//			break;
//		if (Blocks[c].start < ptr)
//		{
//			if (a == c)
//				return NULL;
//			a = c + 1;
//		}
//		else
//		{
//			if (b == c)
//				return NULL;
//			b = c - 1;
//		}
//	}
//
//	if (Blocks[c].lenght >= newSize)
//	{
//		MarkBlock(Blocks[c], MARK_FREE);
//		Blocks[c].lenght = newSize;
//		MarkBlock(Blocks[c], MARK_ALLOC);
//		return ptr;
//	}
//
//	if (CheckForRealloc(ptr, newSize) == 1)
//	{
//
//		Blocks[c].lenght = newSize;
//		MarkBlock(Blocks[c], MARK_ALLOC);
//		return ptr;
//	}
//	else
//	{
//		void* newPtr = AllocMem(newSize);
//		memcpy(newPtr, ptr, newSize);
//		free(ptr);
//		return newPtr;
//	}
//
//}
//
//void PrintBlocks()
//{
//	for (int i = 0; i < BlocksCount; i++)
//		printf("Start: %x Lenght: %u\n", Blocks[i].start, Blocks[i].lenght);
//}
//
//int _tmain(int argc, _TCHAR* argv[])
//{
//	void* a = AllocMem(12);
//	void* b = AllocMem(13);
//	printf("\n%d", &a);
//
//	_getch();
//	return 0;
//}
//
