#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#include<unistd.h>
#include<arpa/inet.h>
#include<sys/socket.h>

#define BUF_SIZE 1024

void error_handling(char *msg);

int main(int argc,char*argv[]){
	int dataSock;
	char message[BUF_SIZE];
	int len,str_len=0,idx=0;
	struct sockaddr_in servAddr;

	dataSock = socket(PF_INET,SOCK_STREAM, 0);

	memset(&servAddr,0,sizeof(servAddr));
	servAddr.sin_family = AF_INET;
	servAddr.sin_addr.s_addr = inet_addr(argv[1]);
	servAddr.sin_port = htons(atoi(argv[2]));

	connect(dataSock,(struct sockaddr *)&servAddr, sizeof(servAddr));
	// 연결요청 후, 연결설정이 완료됨
	while(1){
		fputs("Input message(Q to quit): ", stdout);
		fgets(message, BUF_SIZE, stdin);

		if(!strcmp(message, "q\n") || !strcmp(message, "Q\n"))	break;

		write(dataSock, message, strlen(message));
		str_len = read(dataSock, message, BUF_SIZE - 1);
		message[str_len] = 0;
		printf("Message from server: %s", message);
	while(len = read(dataSock,&msg[idx++],1)){
		if(len==-1){
			error_handling("read() error!");
			break;
		}
		str_len+=len;
	}
	printf("Message from server : %s \n",msg);
	printf("Function read all count : %d\n", str_len);
	close(dataSock);

	return 0;
}

void error_handling(char *msg){
	fputs(msg, stderr);
	fputs("\n", stderr);
	exit(1);
}
