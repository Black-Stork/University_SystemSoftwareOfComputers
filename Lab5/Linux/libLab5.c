#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <aio.h>
#include <sys/stat.h>
#include <dlfcn.h>
#include <fcntl.h>
#include <sys/types.h>
#include <dirent.h>
#include <dlfcn.h>
#include <string.h>

int MyRead( char *lpBuf,int bufSize, char *lpFileName,int offset)
{
    if ( lpBuf == NULL )
        return 0;

    unsigned int	byteReaded;
    struct aiocb 	aiocbStruct = {};
	    aiocbStruct.aio_fildes = open(lpFileName, O_RDONLY);
	    aiocbStruct.aio_offset = offset;			
            aiocbStruct.aio_buf = lpBuf;
            aiocbStruct.aio_nbytes = bufSize;
            aiocbStruct.aio_sigevent.sigev_notify = SIGEV_NONE;
    aio_read( &aiocbStruct );

    while( aio_error( &aiocbStruct ) == EINPROGRESS );

    byteReaded = aio_return( &aiocbStruct );
    close( aiocbStruct.aio_fildes );
    lpBuf[ byteReaded ] = 0;

    if ( byteReaded == 0 )
        return 0;

    return byteReaded + offset;
}

void MyWrite(char *lpBuf, int hFile, unsigned int size)
{
    if ( lpBuf == NULL )
        return;

    struct aiocb 	aiocbStruct = {};
            aiocbStruct.aio_buf = lpBuf;
            aiocbStruct.aio_offset = 0xFFFFFFFF;
            aiocbStruct.aio_fildes = hFile;
            aiocbStruct.aio_nbytes = size;
            aiocbStruct.aio_sigevent.sigev_notify = SIGEV_NONE;
    aio_write( &aiocbStruct );
    while( aio_error(&aiocbStruct) == EINPROGRESS );
	puts("WRITE ");	
	puts(lpBuf);
}

/*
 EINPROGRESS - возвращает AIO_ERROR(3)
 0 - успех
 EOVERFLOW - конец файла


 */

