#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>

#define BUF_SIZE	100
#define MAX_CLNT	256
#define SERVERCNT	2
#define SEND_SIZE_SERINFO	130

struct ServerInfo
{
	char ip[16];
	char port[6];
	char name[30];
	int clnt_cnt;
};


void * handle_clnt(void * arg);
void * send_msg(void  *arg);
void error_handling(char * msg);

int clnt_cnt=0;
int clnt_socks[MAX_CLNT];
pthread_mutex_t mutx;

struct ServerInfo  serverInfo[SERVERCNT] = {
	{"127.0.0.1", "9991", "s1", 0},
	{"127.0.0.1", "9991", "s2", 0},
};

int main(int argc, char *argv[])
{
	int serv_sock, clnt_sock;
	int sserv_sock, sclnt_sock[2];
	struct sockaddr_in serv_adr, clnt_adr;
	int clnt_adr_sz;
	pthread_t t_id1, t_id2;
	if(argc!=2) {
		printf("Usage : %s <port>\n", argv[0]);
		exit(1);
	}
  
	pthread_mutex_init(&mutx, NULL);
	serv_sock=socket(PF_INET, SOCK_STREAM, 0);

	memset(&serv_adr, 0, sizeof(serv_adr));
	serv_adr.sin_family=AF_INET; 
	serv_adr.sin_addr.s_addr=htonl(INADDR_ANY);
	serv_adr.sin_port=htons(atoi(argv[1]));
	
	if(bind(serv_sock, (struct sockaddr*) &serv_adr, sizeof(serv_adr))==-1)
		error_handling("bind() error");
	if(listen(serv_sock, 5)==-1)
		error_handling("listen() error");


	
	while(1)
	{
		clnt_adr_sz=sizeof(clnt_adr);
		clnt_sock=accept(serv_sock, (struct sockaddr*)&clnt_adr,&clnt_adr_sz);
		
		pthread_create(&t_id1, NULL, handle_clnt, (void*)&clnt_sock);
		pthread_create(&t_id2, NULL, send_msg, (void*)&clnt_sock);
		pthread_detach(t_id1);
		pthread_detach(t_id2);

		printf("Connected client IP: %s \n", inet_ntoa(clnt_adr.sin_addr));
	}
	close(serv_sock);
	return 0;
}
	
void * handle_clnt(void * arg)
{
	int clnt_sock=*((int*)arg);
	int str_len=0, i, j;
	char msg[SEND_SIZE_SERINFO];
	char msg2[SEND_SIZE_SERINFO];
	
	for(i = 0;i < 2;i++)
	{
		sprintf(msg, "%s$%s$%s$%d", 
				serverInfo[i].ip, serverInfo[i].port, 
				serverInfo[i].name, serverInfo[i].clnt_cnt);
		str_len = strlen(msg);
	
		sprintf(msg2, "%d$%s", 
				str_len, msg);

		str_len = strlen(msg2);

		puts(msg2);

		write(clnt_sock, msg2, str_len);
	}

	return NULL;
}
void * send_msg(void *arg)   // send to all
{
	int clnt_sock=*((int*)arg);
	int str_len=0, i, j;
	int num;
	
	read(clnt_sock, &num, 4);
	printf("%d \n", num);

	close(clnt_sock);
	return NULL;
	
}
void error_handling(char * msg)
{
	fputs(msg, stderr);
	fputc('\n', stderr);
	exit(1);
}
