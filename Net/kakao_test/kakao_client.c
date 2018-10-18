#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

void error_handling(char *msg);


int main(int argc, char *argv[])
{
	int sock;
	struct sockaddr_in serv_addr;
	int len, i = 0, j = 0;
	int intarr[5] = { 9, 20, 28, 18, 11 };
	char msg[100];
	char chararr[5][5];

	if(argc != 3)
	{
		printf("Usage: %s <IP> <PORT>\n", argv[0]);
		exit(1);
	}

	sock = socket(PF_INET, SOCK_STREAM, 0); //Create Data Socket
	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
	serv_addr.sin_port = htons(atoi(argv[2]));

	connect(sock,(struct sockaddr*)&serv_addr, sizeof(serv_addr));

	printf("Input 5 int:");

	for(i = 0; i < 5; i++)
	{
		scanf("%d", &intarr[i]);
		if(intarr[0] == -1) 
		{
			write(sock, intarr, sizeof(intarr));
			printf("exit\n");
			exit(1);
		}
	}

	write(sock, intarr, sizeof(intarr));
	
	read(sock, chararr, sizeof(chararr));

	for(i = 0; i < 5; i++)
	{
		for(j = 0; j < 5; j++)
		{
			if(chararr[i][j] == '0') printf(" ");
			else if(chararr[i][j] == '1') printf("#");
		}
		printf("\n");
	}

	printf("Print Map done\n");
	close(sock);
	
	return 0;	
}

void error_handling(char *msg)
{
	fputs(msg, stderr);
	fputc('\n', stderr);
	exit(1);
}
