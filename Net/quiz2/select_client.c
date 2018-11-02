#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <time.h>


void error_handling(char *msg);

#define HELLO_SIZE 6
#define NICKNAME_SIZE 10
#define WORD_SIZE 7
#define EX_SIZE 12
#define TIME_SIZE 255
#define RANK_SIZE 5

int main(int argc, char *argv[])
{
	int sock;
	struct sockaddr_in serv_addr;
	int len, i;
	char msg[1024];
	clock_t start, end;

	sock = socket(PF_INET, SOCK_STREAM, 0); //Create Data Socket
	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
	serv_addr.sin_port = htons(atoi(argv[2]));

	connect(sock,(struct sockaddr*)&serv_addr, sizeof(serv_addr));
	
	
	printf("Nickname: ");
	scanf("%s", msg);

	write(sock, msg, NICKNAME_SIZE);
	read(sock, msg, WORD_SIZE + EX_SIZE + 2);
	start = clock();
	printf("%s\n", msg);
	for(i=0;i<2;i++)
	{
		len = read(sock, msg, WORD_SIZE + EX_SIZE + 2);
		printf("%s\n", msg);
	}
	for(i=0;i<3;i++)
	{
		printf("%ds answer: ");
		scanf("%s", msg);
		end = clock();
		strcat(msg, ", ");
		strcat(msg, (char *)(((int)end-start)/1000));
		write(sock, msg, WORD_SIZE + TIME_SIZE + 2);
	}
	
	read(sock, msg, RANK_SIZE);
	printf("rank: %s", msg);

	close(sock);
	
	return 0;	
}

void error_handling(char *msg)
{
	fputs(msg, stderr);
	fputc('\n', stderr);
	exit(1);
}
