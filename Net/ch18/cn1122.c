#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> 
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>
	
#define BUF_SIZE 100
#define NAME_SIZE 20
#define SEND_SIZE_SERINFO	130

struct ServerInfo
{
	char ip[16];
	char port[6];
	char name[30];
	int clnt_cnt;
} sInfo[2];
	
void * send_msg(void * arg);
void * recv_msg(void * arg);
void error_handling(char * msg);
	
char name[NAME_SIZE]="[DEFAULT]";
char msg[BUF_SIZE];

int num;
	
int main(int argc, char *argv[])
{
	int sock;
	struct sockaddr_in serv_addr;
	pthread_t snd_thread, rcv_thread;
	void * thread_return;
	if(argc!=4) {
		printf("Usage : %s <IP> <port> <name>\n", argv[0]);
		exit(1);
	 }

	sprintf(name, "[%s]", argv[3]);
	sock=socket(PF_INET, SOCK_STREAM, 0);

	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family=AF_INET;
	serv_addr.sin_addr.s_addr=inet_addr(argv[1]);
	serv_addr.sin_port=htons(atoi(argv[2]));
  
	if(connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr))==-1)
		error_handling("connect() error");

	//pthread_create(&snd_thread, NULL, send_msg, (void*)&sock);
	pthread_create(&rcv_thread, NULL, recv_msg, (void*)&sock);
	//pthread_join(snd_thread, &thread_return);
	pthread_join(rcv_thread, &thread_return);
	//close(sock);  

	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family=AF_INET;
	serv_addr.sin_addr.s_addr=inet_addr(sInfo[num-1].ip);
	serv_addr.sin_port=htons(atoi(sInfo[num-1].port));
  
	if(connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr))==-1)
		error_handling("connect() error");

	return 0;
}
	
void * send_msg(void * arg)   // send thread main
{
	int sock=*((int*)arg);
	char name_msg[NAME_SIZE+BUF_SIZE];
	while(1) 
	{
		fgets(msg, BUF_SIZE, stdin);
		if(!strcmp(msg,"q\n")||!strcmp(msg,"Q\n")) 
		{
			sprintf(name_msg,"%s %s", name, "left the room\n");
			write(sock, name_msg, strlen(name_msg));
			close(sock);
			exit(0);
		}
		sprintf(name_msg,"%s %s", name, msg);
		write(sock, name_msg, strlen(name_msg));
	}
	return NULL;
}
	
void * recv_msg(void * arg)   // read thread main
{
	int sock=*((int*)arg);
	char name_msg[NAME_SIZE+BUF_SIZE], tmpClntCnt[4];
	int str_len, i, j, k;
	char ch;
	
	for(k=0;k<2;k++)
	{
		for(i=0; i<3; i++) {
		str_len=read(sock, &ch, 1);
		if(str_len==-1) 
			return (void*)-1;
		if(ch != '$') {
			name_msg[i] = ch;
		} else {
			name_msg[i] = '\0';
			break;
		}
	}
	
	puts(name_msg);
	str_len = atoi(name_msg);

	int recvSize = 0, len;
	while(recvSize < str_len) {
		len = read(sock, &name_msg[recvSize], str_len);
		recvSize += len;
	}
	name_msg[str_len] = 0;

	puts(name_msg);

	char *ptr = strtok(name_msg, "$");      // " " 공백 문자를 기준으로 문자열을 자름, 포인터 반환
	i=0;
   	while (ptr != NULL)               // 자른 문자열이 나오지 않을 때까지 반복
    	{
		switch(i) {
		case 0:
			strcpy(sInfo[k].ip, ptr);        // 자른 문자열 사용
		    ptr = strtok(NULL, "$");      // 다음 문자열을 잘라서 포인터를 반환
			i++;		break;
		case 1:
			strcpy(sInfo[k].port, ptr);        // 자른 문자열 사용
		    ptr = strtok(NULL, "$");      // 다음 문자열을 잘라서 포인터를 반환
			i++;		break;
		case 2:
			strcpy(sInfo[k].name, ptr);        // 자른 문자열 사용
		    ptr = strtok(NULL, "$");      // 다음 문자열을 잘라서 포인터를 반환
			i++;		break;
		case 3:
			strcpy(tmpClntCnt, ptr);        // 자른 문자열 사용
		    ptr = strtok(NULL, "$");      // 다음 문자열을 잘라서 포인터를 반환
			i++;		break;
		}
    	}

	sInfo[k].clnt_cnt = atoi(tmpClntCnt);
	
	printf("%d. %10s %6s %3d \n", 
			k+1, sInfo[k].ip, sInfo[k].name, sInfo[k].clnt_cnt);
	}

	puts("select server: ");
	scanf("%d", &num);
	write(sock, &num, 4);


	return NULL;
}
void error_handling(char *msg)
{
	fputs(msg, stderr);
	fputc('\n', stderr);
	exit(1);
}
