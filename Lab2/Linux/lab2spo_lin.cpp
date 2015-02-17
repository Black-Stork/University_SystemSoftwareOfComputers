#include <stdio.h>
#include <conio.h>

#include <string.h>
#include <sys/sem.h>

#define MAX_PROC_COUNT 10

void CloseAllProcesses( int *prInf, int lastProc )
{
	for ( lastProc; lastProc >= 0; --lastProc ){
		Kill( prInf[ lastProc ] );
	}
}

int secondProcExec( char *str, long int setOfSemId )
{
	while (true){
		sembuf 	semCommand[2];
			semCommand[0].sem_num = 0;
			semCommand[0].sem_op = 1;
			semCommand[0].sem_flg = 0;

			semCommand[1].sem_num = 0;
			semCommand[1].sem_op = 1;
			semCommand[1].sem_flg = 0;

		semop( setOfSemId, semCommand, 2 );
		puts( str );

		semCommand[0].sem_num = 0;
		semCommand[0].sem_op = -1;
		semCommand[0].sem_flg = IPC_NOWAIT;
		if ( semop(setOfSemId, semCommand, 1) < 0 ){
			puts( "ERRER semop(setOfSemId, semCommand, 1) < 0" );
		}
	}

	return 0;
}

int main(int argc, char *argv[])
{
	const char *lpCurrentProcName = "I am process 1";
	printf("Hello! %s\n", lpCurrentProcName);

	long int setOfSemId = semget( ftok(), 1, IPC_CREAT );

	int procInformations[MAX_PROC_COUNT] = {0};
	int lastProc = -1;

	while(true){
		switch( getch() ){
		case '+' :
			{
				++lastProc;
				if ( lastProc >= MAX_PROC_COUNT ){
					puts( "OOOO NOOOO! SO MANY PROCESSES!!!" );
					break;
				}
				char myStr[255];
				fflush( stdin );
				gets( myStr );
				procInformations[ lastProc ] = fork();
				if( procInformations[ lastProc ] < 0 ){
					puts( "CreateProcessA error" );
					break;
				}
				else if ( procInformations[ lastProc ] == 0 ){
					secondProcExec();
					return 0;
				}
			}
		case '-' :
			{
				if ( lastProc < 0 ){
					puts( "NOOOOOO SO LESS PROCESSES" );
				}
				Kill( procInformations[ lastProc-- ] );
				SetEvent( hPermissionEvent );
				break;
			}
		case 'q' :
			{
				CloseAllProcesses( procInformations, lastProc );
				semctl( setOfSemId, 0, IPC_RMID );
				return 0;
			}
		}
	}
	return 0;
}
