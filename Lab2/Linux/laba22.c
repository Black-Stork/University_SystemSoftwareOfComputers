#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

#define COUNT 10

char *str[COUNT] =
{
	  "0000", "11111",
	  "222222", "3333333",
	  "44444444", "55555555",
	  "6666666666", "77777777777",
	  "888888888888", "9999999999999"
};

int printFlag = 0;
int flagEnd = 1;
int proc = 0;

struct sigaction printSignal, endSignal;//функция обработчика сигналов

void EndProcesses(pid_t cpid[]);
void PrintString();

void ProcessOverfolw();
void ProcessFailedError();

void canPrint(int signo)
{
  printFlag = 1;
}
void setEndFlag(int signo)
{
  flagEnd = 1;
}



int main(void)
{
 
  int i = 0;

  int flag = 0;
  char c = 0;

  pid_t cpid[MAX_COUNT];

  // инициализация (должна быть выполнена 
  // перед использованием ncurses)

  initscr();//Определив размеры экрана терминала,
  //Функция noecho отключает автоматическое отображение при вводе.
  //  Это нужно, если программист сам хочет решать вывоить ему полученный символ или нет. 
  noecho();

  printSignal.sa_handler = canPrint; //указатель на функцию-обработчик
  //указатель на функцию со  след. прототипом: void handler (int signum0
  if (sigaction(SIGUSR1,&printSignal,NULL)<0)//номер сигнала, новое действия связанное с сигналом SIGUSR1,
  {
	  printw("Sigaction error");
	  refresh();
	  return 1;
  }

  endSignal.sa_handler = setEndFlag;
  if (sigaction(SIGUSR2,&endSignal,NULL)<0)//доч.
  {
	  printw("Sigaction error");
	  refresh();
	  return 1;
  }

  while(c != 'q')
  {
    c = getch();

    switch(c)
    {
     case '+':
	if(proc < MAX_COUNT)
	{
	  cpid[proc] = fork();
	  proc++;
	  switch(cpid[proc-1])
	  {
	    case 0:
	      flagEnd = 0;
	      while(!flagEnd)
	      {
			usleep(10000);
			if(printFlag)
			{
			  PrintString();

			  printFlag = 0;
			  kill(getppid(),SIGUSR2);//сигнал радитель. о финише печати
			}
	      }
	      return 0;
	      break;

	    case -1:
		 ProcessFailedError();
	     break;

	    default:
	      break;
	  }
	}
	else ProcessOverfolw();

	break;

      case '-':
	if(proc > 0)
	{
	  kill(cpid[proc-1],SIGUSR2);//заканчивая последным дочерным процессам
	  waitpid(cpid[proc-1],NULL,0);//ожидает завершения процесса  

	  proc--;

	  if (i >= proc) 
	  {
	    i = 0;
	    flag = 1;
	    flagEnd = 1;
	  }

	}
	    break;

	    case 'q':
	    EndProcesses(cpid);
	    clear();
		endwin();
	    return 0;
    }

    if(flagEnd && proc>0)  // если текущий процесс завершения печати
    {
      flagEnd = 0;
      if(i >= proc - 1)// Если текущий номер> количество процессов,
	  i = 0;
      else if(!flag) i++; // затем начать с самого начала

      flag = 0;

      kill(cpid[i],SIGUSR1);// сигнал к доч процесса о печати строки; в начале I=1
    }
    refresh();
  }
  
  return 0;
}

void ProcessFailedError()
{
   printw("Fork failed");
   refresh();
}

void ProcessOverfolw()
{
	printw("Sorry, it support only 10 processes");
	refresh();
}

void EndProcesses(pid_t cpid[])
{
	if(cpid[proc-1]!=0)
  {
    while(proc > 0)
    {
      kill(cpid[proc-1],SIGUSR2);
      waitpid(cpid[proc-1],NULL,0);
      proc--;
    }
  }
}

void PrintString()
{
	int j = 0;
	for(j=0; j<strlen(strings[proc-1]); j++)
		  {
		    if(flagEnd)
		      return;

		    printw("%c",strings[proc-1][j]);
		    refresh();
		    usleep(70000);
		  }
}


