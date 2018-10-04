#lw:include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
void dead_child(int v){
	int s;
	wait(&s);
	printf("child p is over\n");
}

int gval = 10;
int main()
{
	int status;
	pid_t pid;
	int reP;

	signal(SIGCHLD, dead_child);
	

	pid = fork();

	if(pid == 0)
	{
		//자식 프로세스 영역
		sleep(10);
		
	}
	else
	{
		sleep(100);
	}


	
	return 0;
}

