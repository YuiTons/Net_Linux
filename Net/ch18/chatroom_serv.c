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
#define MAX_CHATROOM 10

struct roominfo{
	int memcount;
	char name[BUF_SIZE];
};

struct Friend{
	char friendList[10][20];
	int friendState[10];
	char userName[10];
};

void * handle_clnt(void * arg);
void send_msg(char * msg, int len, int sock, int room);
void error_handling(char * msg);

int clnt_cnt=0;
int clnt_socks[MAX_CLNT][2];
struct roominfo ri[MAX_CHATROOM];
struct Friend ob;
pthread_mutex_t mutx;

int main(int argc, char *argv[])
{
	int serv_sock, clnt_sock;
	struct sockaddr_in serv_adr, clnt_adr;
	int clnt_adr_sz;
	pthread_t t_id;
	if(argc!=2) {
		printf("Usage : %s <port>\n", argv[0]);
		exit(1);
	}
  
	strcpy(ob.friendList[0], "f1");
	strcpy(ob.friendList[1], "f2");
	strcpy(ob.friendList[2], "f3");
	strcpy(ob.friendList[3], "f4");
	strcpy(ob.friendList[4], "f5");

	ob.friendState[2] = 1;
	
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
		
		pthread_mutex_lock(&mutx);
		clnt_socks[clnt_cnt++][0] = clnt_sock;
		pthread_mutex_unlock(&mutx);
		// 1
		pthread_create(&t_id, NULL, handle_clnt, (void*)&clnt_sock);
		pthread_detach(t_id);

		printf("Connected client IP: %s \n", inet_ntoa(clnt_adr.sin_addr));
	}
	close(serv_sock);
	return 0;
}
	
void * handle_clnt(void * arg)
{
	// 2
	int i, rnum = 0;
	int clnt_sock = *((int*)arg);
	int len = 0;
	char buf[1024];


	for(i=0;i<MAX_CHATROOM;i++)
	{
		if(ri[i].memcount>0)
		{
			sprintf(buf, "%d: %s, %d\n", i, ri[i].name, ri[i].memcount);
			write(clnt_sock, buf, strlen(buf));
		}
		else
		{
			sprintf(buf, "%d: empty\n", i);
			write(clnt_sock, buf, strlen(buf));
		}
	}

	read(clnt_sock, buf, BUF_SIZE);
	if(('0'<=buf[0])&&(buf[0]<='9'))
	{
		rnum = buf[0] - '0';

		printf("%d selected\n", buf[0] - '0');
		pthread_mutex_lock(&mutx);
		for(i=0;i<clnt_cnt;i++)
		{
			if(clnt_socks[i][0] == clnt_sock)
				clnt_socks[i][1] = rnum;
		}
		pthread_mutex_unlock(&mutx);

		if(ri[rnum].memcount > 0)
			ri[rnum].memcount++;
		else
		{
			strcpy(buf, "set chatroom name");
			write(clnt_sock, buf, strlen(buf)); 

			read(clnt_sock, buf, BUF_SIZE);
			strcpy(ri[rnum].name, buf);

			sprintf(buf, "%d: %s, %d\n", rnum, ri[rnum].name, ri[i].memcount);
			write(clnt_sock, buf, strlen(buf));
		}
	}
/*

	for(i=0;i<10;i++)
	{
		write(clnt_sock, ob.friendList[i], strlen(ob.friendList[i]);
	}

	for(i=0;i<10;i++)
	{
		write(clnt_sock, (char*)&ob.friendState[i], 4);
	}
*/
	while(1)
	{
		len = read(clnt_sock, buf, sizeof(buf));
		if(!len) break;
		
		send_msg(buf, len, clnt_sock, rnum);
	}

	pthread_mutex_lock(&mutx);
	for(i=0;i<clnt_cnt;i++)
	{
		if(clnt_socks[i][0] == clnt_sock)
		{
			while(i<clnt_cnt)
			{
				clnt_socks[i][0] = clnt_socks[i+1][0];
				clnt_socks[i][1] = clnt_socks[++i][1];
			}
			break;
		}
	}
	clnt_cnt--;
	pthread_mutex_unlock(&mutx);
	close(clnt_sock);

	return NULL;
}

void send_msg(char * msg, int len, int sock, int room)   // send to all
{
	// 3
	int i;

	pthread_mutex_lock(&mutx);
	for(i=0;i<clnt_cnt;i++)
	{
		if(sock == clnt_socks[i][0]) continue;
		if(clnt_socks[i][1] == room)
			write(clnt_socks[i][0], msg, len);
	}
	pthread_mutex_unlock(&mutx);
}

void error_handling(char * msg)
{
	fputs(msg, stderr);
	fputc('\n', stderr);
	exit(1);
}
