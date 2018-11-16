#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>

#define BUF_SIZE 100
#define MAX_CLNT 256
#define SERVERCNT 2
#define SEND_SIZE_SERINFO 127

struct ServerInfo{
	char ip[16];
	char port[6];
	char name[30];
	int clnt_cnt;
};

void * handle_clnt(void * arg);
void send_msg(char * msg, int len, int clnt_sock);
void error_handling(char * msg);

int clnt_cnt=0;
int clnt_socks[MAX_CLNT];
pthread_mutex_t mutx;

struct ServerInfo serverinfo[SERVERCNT] = {
	{"127.0.0.1", "9991", "s1", 0},
	{"127.0.0.1", "9992", "s2", 0}
};

int main(int argc, char *argv[])
{
	int serv_sock, clnt_sock;
	int sserv_sock, sclnt_sock[2];
	struct sockaddr_in serv_adr, clnt_adr;
	struct sockaddr_in sserv_adr, sclnt_adr;
	int clnt_adr_sz;
	pthread_t t_id;
	if(argc!=3) {
		printf("Usage : %s <port> <port>\n", argv[0]);
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
/*
	memset(&sserv_adr, 0, sizeof(sserv_adr));
	sserv_adr.sin_family=AF_INET;
	sserv_adr.sin_addr.s_addr=htonl(INADDR_ANY);
	sserv_adr.sin_port=htons(atoi(argv[2]));

	if(bind(sserv_sock, (struct sockaddr*) &sserv_adr, sizeof(sserv_adr)) == -1)
		error_handling("bind() error");
	
	if(listen(sserv_sock, 5)==-1)
		error_handling("listen() error");
*/
/*	
	for(i=0; i<2; i++)
	{
		clnt_adr_sz = sizeof(clnt_adr);
		sclnt_sock[i] = accpet(sserv_sock, (struct sockaddr*)&clnt_adr, &clnt_adr_sz);

	}
*/
	while(1)
	{
		clnt_adr_sz=sizeof(clnt_adr);
		clnt_sock=accept(serv_sock, (struct sockaddr*)&clnt_adr,&clnt_adr_sz);

		pthread_create(&t_id, NULL, handle_clnt, (void*)&clnt_sock);
		pthread_detach(t_id);
		printf("Connected client IP: %s \n", inet_ntoa(clnt_adr.sin_addr));
	}

	close(serv_sock);
	return 0;
}
	
void * handle_clnt(void * arg)
{
	int clnt_sock=*((int*)arg);
	int str_len=0, i, j;
	char msg[BUF_SIZE];
	char sinfo[SEND_SIZE_SERINFO];

	for(i=0;i<2;i++)
	{
		sprintf(sinfo, "%s$%s$%s$%d", serverinfo[i].ip, serverinfo[i].port, serverinfo[i].name, serverinfo[i].clnt_cnt);
		str_len = strlen(sinfo);
		sprintf(msg, "%d$%s", str_len, sinfo);
		write(clnt_sock, msg, strlen(msg));
	}

/*
	while((str_len=read(clnt_sock, msg, sizeof(msg)))!=0)
		send_msg(msg, str_len, clnt_sock);	

	pthread_mutex_lock(&mutx);
	for(i=0; i<clnt_cnt; i++)   // remove disconnected client
	{
		if(clnt_sock==clnt_socks[i])
		{
			for(j=i; j<clnt_cnt-1; j++)
				clnt_socks[j]=clnt_socks[j+1];
			break;
		}
	}
	clnt_cnt--;
	pthread_mutex_unlock(&mutx);
*/
	close(clnt_sock);
	return NULL;
}
void send_msg(char * msg, int len, int clnt_sock)   // send to all
{
	int i;

	pthread_mutex_lock(&mutx);
	for(i=0; i<clnt_cnt; i++) {
		if(clnt_sock != clnt_socks[i])
			write(clnt_socks[i], msg, len);
	}
	pthread_mutex_unlock(&mutx);
}
void error_handling(char * msg)
{
	fputs(msg, stderr);
	fputc('\n', stderr);
	exit(1);
}
