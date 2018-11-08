#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUF_SIZE			1024
#define HEAD_LEN			5

void error_handling(char *message);

int main(int argc, char *argv[])
{
	int			sock;
	struct		sockaddr_in serv_adr;

	char		buf[BUF_SIZE];
	int			readLen, tmpLen, index;

	if(argc!=3) {
		printf("Usage : %s <IP> <port>\n", argv[0]);
		exit(1);
	}
	
	sock=socket(PF_INET, SOCK_STREAM, 0);   
	if(sock==-1)
		error_handling("socket() error");
	
	memset(&serv_adr, 0, sizeof(serv_adr));
	serv_adr.sin_family=AF_INET;
	serv_adr.sin_addr.s_addr=inet_addr(argv[1]);
	serv_adr.sin_port=htons(atoi(argv[2]));
	
	if(connect(sock, (struct sockaddr*)&serv_adr, sizeof(serv_adr))==-1)
		error_handling("connect() error!");
	else
		puts("Connected...........");

	
	tmpLen = 0;
	while(1) {
		char bufT[1];
		readLen = read(sock, bufT, 1);
		tmpLen += readLen;
		buf[index++] = bufT[0];
		if(tmpLen == HEAD_LEN) {
			buf[index] = 0;
			break;
		}
	}	
	
	close(sock);
	return 0;
}

void error_handling(char *message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}