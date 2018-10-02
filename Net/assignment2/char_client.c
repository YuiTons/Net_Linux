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

	int opnd_cnt, recv_len, recv_cnt;
	char opmsg[BUF_SIZE];
	char opinfo[BUF_SIZE];


	dataSock = socket(PF_INET, SOCK_STREAM, 0);

	memset(&servAddr, 0, sizeof(servAddr));
	servAddr.sin_family = AF_INET;
	servAddr.sin_addr.s_addr = inet_addr(argv[1]);
	servAddr.sin_port = htons( atoi(argv[2]) );

	if(connect(dataSock, (struct sockaddr*)&servAddr, sizeof(servAddr)) == -1) 
		error_handling("connect() error!");
	else
		puts("Connected");

	for(i = 0; i < 3; i++)
	{
		fputs("String length : ", stdout);
		scanf("%d", (int*)&opmsg);
	
		fputs("String : ", stdout);
		scanf("%s", &opmsg[4]);

		fgetc(stdin);
		len = *(int*)opmsg;

		write(dataSock, opmsg, len + 4);
		
		opnd_cnt = 0;
		read(dataSock, &opnd_cnt, 4);
		recv_len = 0;

		while(opnd_cnt > recv_len)
		{
			recv_cnt = read(dataSock, &opinfo[recv_len], 1);
			if(recv_cnt == -1) error_handling("recv read() error!");

			recv_len += recv_cnt;
		}
		opinfo[recv_len] = '\0';
		printf("server: %s \n", opinfo);
		//read(dataSock, &result, BUF_SIZE);
		//printf("server: %s", result);
	}

	close(dataSock);

	return 0;
}

void error_handling(char *msg) {
	fputs(msg, stderr);
	fputc('\n', stderr);
	exit(1);
}
