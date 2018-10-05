#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#include<unistd.h>
#include<arpa/inet.h>
#include<sys/socket.h>

void error_handling(char *msg);

int main(int argc,char*argv[]){
	int dataSock;
	char msg[100];
	int len,str_len=0,idx=0;
	struct sockaddr_in servAddr;

	dataSock = socket(PF_INET,SOCK_STREAM, 0);

	memset(&servAddr,0,sizeof(servAddr));
	servAddr.sin_family = AF_INET;
	servAddr.sin_addr.s_addr = inet_addr(argv[1]);
	servAddr.sin_port = htons(atoi(argv[2]));

	connect(dataSock,(struct sockaddr *)&servAddr, sizeof(servAddr));
	// 
	//len = read(sock, msg, sizeof(msg)-1);
	
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
