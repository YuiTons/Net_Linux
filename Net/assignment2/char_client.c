#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUF_SIZE 1024
#define OPSZ	4
#define RLT_SIZE 4 

void error_handling(char *msg);

int main(int argc, char *argv[]){
	int dataSock;
	struct sockaddr_in  servAddr;

	char message[100];
	int len, idx=0, str_len=0, read_len=0, i;

	int opnd_cnt;
	char opmsg[BUF_SIZE];
	int result;


	dataSock = socket(PF_INET, SOCK_STREAM, 0);

	memset(&servAddr, 0, sizeof(servAddr));
	servAddr.sin_family = AF_INET;
	servAddr.sin_addr.s_addr = inet_addr(argv[1]);
	servAddr.sin_port = htons( atoi(argv[2]) );

	if(connect(dataSock, (struct sockaddr*)&servAddr, sizeof(servAddr)) == -1) 
		error_handling("connect() error!");
	else
		puts("Connected");

	fputs("String length : ", stdout);
	scanf("%d", (int*)&opmsg);
	
	fputs("String : ", stdout);
	scanf("%s", &opmsg[4]);

	fgetc(stdin);
	
	opnd_cnt = strlen(opmsg);
	write(dataSock, opmsg, opnd_cnt*OPSZ+2);
	read(dataSock, &result, RLT_SIZE);

	printf("Operation result : %d \n", result);
	close(dataSock);

	return 0;
}

void error_handling(char *msg) {
	fputs(msg, stderr);
	fputc('\n', stderr);
	exit(1);
}
