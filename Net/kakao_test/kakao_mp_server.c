#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <fcntl.h>

void read_childproc(int sig);
void error_handling(char *meg);

int main(int arpc, char *argv[])
{
	int serv_sock, clnt_sock;
	struct sockaddr_in serv_addr;
	struct sockaddr_in clnt_addr;
	socklen_t clnt_addr_size;
	char message[]="Hello World!";
	int intarr[5] = { 0 };
	char chararr[5][5];
	int i = 0, j = 0;

	pid_t pid;
	struct sigaction act;
	int state;

	act.sa_handler = read_childproc;
	sigemptyset(&act.sa_mask);
	//act.sa_flags = 0;
	//state = sigaction(SIGCHLD, &act, 0);

	serv_sock = socket(PF_INET, SOCK_STREAM, 0); //Create Listen Socket
	if(serv_sock == -1) error_handling("socket() error");
	
	memset(&serv_addr, 0 ,sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr =  htonl(INADDR_ANY);
	serv_addr.sin_port = htons(atoi(argv[1]));

	if(bind(serv_sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1)
		error_handling("bind() error");
	while(1)
	{
		if(listen(serv_sock, 5) == -1)
			error_handling("listen() error");

		clnt_addr_size = sizeof(clnt_addr);
		clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_addr, &clnt_addr_size); //Create Data Socket
		if(clnt_sock == 1)
			error_handling("bind() error");

		pid = fork();
		if(pid == 0)
		{
			close(serv_sock);
			read(clnt_sock, intarr, sizeof(intarr)); 

			if(intarr[0] == -1)
			{
				printf("exit from client\n");
				break;
			}

			for (i = 0; i < 5; i++)
			{
				for (j = 0; j < 5; j++)
				{
					if(intarr[i] % 2)
						chararr[i][4-j] = '1';
					else
						chararr[i][4-j] = '0';

					intarr[i] /= 2;
				}
			}

			write(clnt_sock, chararr, sizeof(chararr));

			close(clnt_sock);
			return 0;
		}
		else
			close(clnt_sock);
	}

	close(serv_sock);
	return 0;

}

void read_childproc(int sig)
{
	pid_t pid;
	int status;
	pid = waitpid(-1, &status, WNOHANG);
	printf("removed proc id: %d\n", pid);
}

void error_handling(char *meg)
{
	fputs(meg, stderr);
	fputs("\n", stderr);
	exit(1);
}
