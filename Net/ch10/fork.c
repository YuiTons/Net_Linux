#include <unistd.h>
#include <stdio.h>

int gval = 10;
int main()
{
	int lval = 5;
	pid_t pid;

	pid = fork();

	if(pid == 0)
	{
		//자식 프로세스 영역
		lval++;
		printf("child process : %d %d \n", gval, lval);
	}
	else
	{
		//부모 프로세스 영역
		gval++;
		printf("parent process : %d %d \n", gval, lval);
	}


	
	return 0;
}

