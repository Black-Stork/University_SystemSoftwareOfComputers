#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <aio.h>
#include <sys/stat.h>
#include <dlfcn.h>
#include <fcntl.h>
#include <sys/types.h>
#include <dirent.h>
#include <dlfcn.h>

int iter = 0;

#define BOOL bool
#define DWORD unsigned int
#define CHAR  char
#define CROSS_THREAD pthread_t
#define CROSS_LOCKER pthread_mutex_t
#define CROSS_HFILE int
#define CROSS_LIB void *
#define CROSS_GET_PROC(dll,funcName) dlsym(dll, funcName)
#define THREAD_RETURN void *


#define MAX_BUF_LEN 1
typedef DWORD(ReadFuncType)(CHAR *, DWORD, CHAR *, DWORD);
typedef void(WriteFuncType)(CHAR *, CROSS_HFILE, DWORD);

struct MyThreadArgs
{
    char			 fName[MAX_BUF_LEN];
    CROSS_LOCKER	 *hWriteEventEnd;
    CROSS_LOCKER	 *hReadEventEnd;
    char			 strData[MAX_BUF_LEN + 1];
    bool			 bExit;
    WriteFuncType	*lpWriteFunc;
    DWORD 		size;	
};

void CrossWait( CROSS_LOCKER *locker ){

    pthread_mutex_lock( locker );
}

void CrossWaitThread( CROSS_THREAD thread ){
    pthread_join( thread, NULL );
}

void CrossDispose( CROSS_LOCKER *locker ){
    pthread_mutex_unlock( locker );
}


void CrossClose( int handle ){
    close( handle );
}
void CrossClose( pthread_mutex_t *lpMutex ){
    pthread_mutex_destroy( lpMutex );
}

CROSS_THREAD CrossCreateThread( void *(*lpFunc)(void *), MyThreadArgs *lpArgs ){
    CROSS_THREAD hThread_read;
    pthread_create( &hThread_read, NULL, lpFunc, lpArgs );
    return hThread_read;
}


CROSS_LOCKER CrossCreateLocker(){
    pthread_mutex_t locker;
    pthread_mutex_init( &locker, NULL );
    pthread_mutex_lock( &locker );
    return locker;
}

CROSS_HFILE CrossOpenFileW(char *fName ){

    return open(fName, O_WRONLY | O_CREAT | O_APPEND |O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
}

CROSS_LIB CrossLoadLib(){
    return dlopen("./libLab5.so", RTLD_NOW);
}

void CrossFreeLib( CROSS_LIB myLib ){
    dlclose( myLib );
}

void CrossCopyStr( char *dest, char *sourse, int len ){
	strncpy(dest, sourse, len);
}


CROSS_LIB gMyDll;

THREAD_RETURN WriterThread(void * args){
    MyThreadArgs *myArgs = static_cast< MyThreadArgs* >(args);
    if (myArgs == NULL){
        puts("Invalid myArgs");
        return 0;
    }

    CROSS_HFILE hFile = CrossOpenFileW(myArgs->fName);
    if ( hFile == 0 ){
        puts("Invalid out file name");
        return 0;
    }

    while ( !myArgs->bExit ){
        puts("WriterThread is waiting reader");
        CrossWait( myArgs->hReadEventEnd );		
        puts("WriterThread isnt waiting reader");
        (*myArgs->lpWriteFunc)( myArgs->strData, hFile, myArgs->size );
        CrossDispose( myArgs->hWriteEventEnd );
    }
    return EXIT_SUCCESS;
}

int main(int argc, char *argv[])
{	
    puts("HI! I am a ReaderThread");
    if (argc <= 2){
        puts("Invalid args\nFirst arg -- output file\nElse args -- list of input files");
        return 1;
    }
    gMyDll = CrossLoadLib();
    ReadFuncType *readFunc = reinterpret_cast< ReadFuncType * >( CROSS_GET_PROC(gMyDll, "MyRead") );

    CROSS_LOCKER hWriteEventEnd	= CrossCreateLocker();
    CROSS_LOCKER hReadEventEnd	= CrossCreateLocker();	

    MyThreadArgs	args = {};
                    args.hWriteEventEnd = &hWriteEventEnd;
                    args.hReadEventEnd = &hReadEventEnd;
                    args.strData[0] = 0;
                    args.bExit = false;
                    args.lpWriteFunc = reinterpret_cast< WriteFuncType * >( CROSS_GET_PROC(gMyDll, "MyWrite") );
		    iter++;	
	CrossCopyStr(args.fName, argv[1], MAX_BUF_LEN);

    CROSS_THREAD hWriteThread = CrossCreateThread(&WriterThread, &args);
    for (int i = 2; i < argc; ++i){
        DWORD newOffset = 0;
	DWORD size = 0;
        do{
		printf("Reading... ");
		size = newOffset;
            newOffset = (*readFunc)(args.strData, MAX_BUF_LEN , argv[i], newOffset);
	   	args.size = newOffset - size;	
		printf("READ !!!!! = %s\n", args.strData);
		printf("newOffset = %d\n", newOffset);
            if (newOffset == 0)
                break;

            CrossDispose(&hReadEventEnd);
            puts("Reader wait");
            CrossWait(&hWriteEventEnd);
            puts("Reader not wait");
        } while (newOffset > 0);			
    }
    puts("EXIT");
    printf(" !!!!!!! %d =   ", iter);	
    args.bExit = true;
    CrossDispose(&hReadEventEnd);
    CrossWaitThread(hWriteThread);
    CrossFreeLib( gMyDll );
    CrossClose(&hWriteEventEnd);
    CrossClose(&hReadEventEnd);
    CrossClose(hWriteThread);
    return EXIT_SUCCESS;
}
