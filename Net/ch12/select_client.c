#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

void error_handling(char *msg);

#define BUF_SIZE 1024

int main(int argc, char *argv[])
{
	int sock;
	struct sockaddr_in serv_addr;
	int len;
	char msg[BUF_SIZE];

	sock = socket(PF_INET, SOCK_STREAM, 0); //Create Data Socket
	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
	serv_addr.sin_port = htons(atoi(argv[2]));

	connect(sock,(struct sockaddr*)&serv_addr, sizeof(serv_addr));
	
	while(1)
	{
		printf("Message: ");
		scanf("%s", msg);
		
		if((strcmp(msg, "q") == 0)||(strcmp(msg, "Q") == 0))
			break;

		write(sock, msg, sizeof(msg));
		len = read(sock, msg, BUF_SIZE);

		printf("Message from server: %s \n",msg);
	}
	close(sock);
	
	return 0;	
}

void error_handling(char *msg)
{
	fputs(msg, stderr);
	fputc('\n', stderr);
	exit(1);
}
