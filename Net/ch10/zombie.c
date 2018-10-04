#include <unistd.h>
#include <stdio.h>
#include <sys/wait.h>

int gval = 10;
int main()
{
	int status;
	pid_t pid;
	int reP;

	pid = fork();

	if(pid == 0)
	{
		//자식 프로세스 영역
		sleep(10);
		return 3;
	}
	else
	{
		//부모 프로세스 영역
		pid = fork();
		if(pid == 0)
		{
			exit(0);
		}
		else
		{
			if(WIFEXITED(status))	printf("Child send %s\n", WEXITSTATUS(status));
			wait(&status);
			if(WIFEXITED(status))	printf("Child send two %d \n", WEXITSTATUS(status));
			sleep(30);
		}

	}


	
	return 0;
}

