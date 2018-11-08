#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUF_SIZE			1024
#define S1_HEAD_LEN			2
#define SERVICE_NUM			1

void error_handling(char *message);

int main(int argc, char *argv[])
{
	int			sock;
	struct		sockaddr_in serv_adr;

	char		buf[BUF_SIZE];
	char		bufT[10];
	int		readLen = 0, recvLen = 0, recvCnt = 0, tmpLen = 0, index = 0;
	int		size = 0;

	if(argc!=4) {
		printf("Usage : %s <IP> <port> <service>\n", argv[0]);
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
	index = 0;
	while(1) {
		char bufT[1];
		readLen = read(sock, bufT, 1);
		tmpLen += readLen;
		buf[index++] = bufT[0];
		if(tmpLen == S1_HEAD_LEN) {
			buf[index] = 0;
			break;
		}
	}

	readLen = atoi(&buf[1]);
	recvLen = 0;
	while(recvLen < readLen)
	{
		recvCnt = read(sock, &buf[recvLen], readLen);
		recvLen += recvCnt;
	}
	buf[recvLen] = 0;
	puts(buf);

	sprintf(bufT, "%s%s", "service", argv[3]);
	size = strlen(bufT);
	sprintf(buf, "%s%s", argv[3], size);
	sprintf(buf, "%s%s", buf, bufT);
	write(sock, buf, sizeof(buf));
	
	close(sock);
	return 0;
}

void error_handling(char *message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1); }
