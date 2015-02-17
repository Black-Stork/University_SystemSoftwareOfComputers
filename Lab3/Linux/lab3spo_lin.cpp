#include "GlobalData.h"

void waitSemAndLock(int setOfSemId){
	sembuf 	semCommand[2];
			semCommand[0].sem_num = 0;
			semCommand[0].sem_op = 0;
			semCommand[0].sem_flg = 0;

			semCommand[1].sem_num = 0;
			semCommand[1].sem_op = 1;
			semCommand[1].sem_flg = 0;

	semop(setOfSemId, semCommand, 2);
}

void unlockSem(int setOfSemId){
	sembuf 	semCommand;
			semCommand.sem_num = 0;
			semCommand.sem_op = -1;
			semCommand.sem_flg = 0;
	semop(setOfSemId, &semCommand, 1);
}

int main(int argc, char *argv[])
{
	puts("Hi! I am proc1");
	
	long int setOfSemId = semget(ftok(argv[0], 18), SEM_COUNT, IPC_CREAT);
	
	int pipesId[2] = {};
	if (pipe(pipesId) < 0){
		puts("pipe(filedes) ERROR");
	}

	int procId = fork();
	if (procId < 0){
		puts("fork() ERROR!");
	}
	else if (procId == 0){		// Сын
		puts("Hi! I am proc2");
		close( pipesId[1] );
		char chBuf = 0;
		while (true){
			waitSemAndLock(setOfSemId);
			
			bool bExit = true;
			while (read(pipesId[0], &chBuf, 1) > 0){
				write(STDOUT_FILENO, &chBuf, 1);
				bExit = false;
			}
			write(STDOUT_FILENO, "\n", 1);

			if (bExit){
				close( pipesId[0] );
				return 0;
			}
			unlockSem(setOfSemId);
		}
		return 0;
	}
	else{						// Родитель
		close( pipesId[0] );
		char inpStr[BUF_SIZE] = { 0 };
		do{
			waitSemAndLock(setOfSemId);

			fflush(stdin);
			fgets(inpStr, BUF_SIZE, stdin);
			write(pipesId[1], inpStr, strlen(inpStr));
			
			unlockSem(setOfSemId);
		} while (inpStr[0] != 0);
		close( pipesId[1] );
		//kill(procId, SIGKILL);
		wait(NULL);
		semctl(setOfSemId, 0, IPC_RMID);
	}
	return 0;
}