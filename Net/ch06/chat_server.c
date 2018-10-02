#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUF_SIZE 30
void error_handling(char *message);


int main(int argc, char *argv[])
{
	int serv_sock;
	int errM, str_len;
	char message[BUF_SIZE];
	struct sockaddr_in serv_adr, clnt_adr, from_adr;
	int clnt_adr_sz;
	if(argc != 2) { printf("Usage : %s <port> \n", argv[0]); exit(1);	}
	
	serv_sock = socket(PF_INET, SOCK_DGRAM, 0);
	if(serv_sock == -1) error_handling("socket() error");
	
	memset(&serv_adr, 0, sizeof(serv_adr));
	serv_adr.sin_family = AF_INET;
	serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_adr.sin_port = htons(atoi(argv[1]));
	
	errM = bind(serv_sock, (struct sockaddr*)&serv_adr, sizeof(serv_adr));
	if(errM == -1) error_handling("bind() error!");
	
	clnt_adr_sz = sizeof(clnt_adr);
	while(1)
	{
		str_len = recvfrom(serv_sock, message, BUF_SIZE, 0, (struct sockaddr*)&clnt_adr, &clnt_adr_sz); 	
		sendto(serv_sock, message, str_len, 0, (struct sockaddr*)&clnt_adr, clnt_adr_sz); 	

		printf("Message from client: %s \n", message);

		fputs("Insert message(q to quit): ",stdout);
		fgets(message, sizeof(message), stdin);
		if((!strcmp(message, "q\n")||!strcmp(message, "Q\n"))) break;

		sendto(serv_sock, message, strlen(message), 0, (struct sockaddr*)&clnt_adr, clnt_adr_sz); 	
		str_len = recvfrom(serv_sock, message, BUF_SIZE, 0, (struct sockaddr*)&clnt_adr, &clnt_adr_sz); 	
		
	}
	
	close(serv_sock);
	
	
	return 0;
}

void error_handling(char *msg)
{
	fputs(msg, stderr);
	fputc('\n', stderr);
	exit(1);
}
