#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <time.h>
#include <signal.h>
#include <wait.h>

#define BUF_SIZE	1024
#define LISTEN_IP	"127.0.0.1"

void read_int(int sig);
void read_childproc(int sig);
void error_handling(char *message);
void initAddr(struct sockaddr_in* serv_adr, char* ip, char* port);
void initSock(int* serv_sock, struct sockaddr_in* serv_adr, int adr_sz);

int flag = 1;
int fds[2];
//int fds[2][2];
int clnt_sock_arr[2] = {-1, -1};

int main(int argc, char *argv[])
{
	int serv_sock, clnt_sock;
	
	struct sockaddr_in serv_adr, clnt_adr;
	socklen_t adr_sz;
	char ip[20] = {0, };
	char port[10] = {0, };

	pid_t pid;
	char buf[BUF_SIZE];
	int str_len, i;
	int Client_Count = 0;
	
	struct sigaction act, act2;
	int state, state2;

	time_t timer;
	struct tm *t;

	act.sa_handler = read_childproc;
	sigemptyset(&act.sa_mask);
	act.sa_flags = 0;
	state=sigaction(SIGCHLD, &act, 0);

	act2.sa_handler = read_int;
	sigemptyset(&act2.sa_mask);
	act2.sa_flags = 0;
	state2=sigaction(SIGINT, &act2, 0);

	pipe(fds);
	pid = fork();
	if(pid == 0) {
		//pipe(fds[Client_Count]);
		FILE *fp = fopen("log.txt", "at");
		char msgbuf[BUF_SIZE];
		int i, len;
		struct sigaction act;

		while(1){
			len = read(fds[0], msgbuf, BUF_SIZE);
			if( strcmp(msgbuf, "exit") == 0) {
				break;
			}
			fwrite((void*)msgbuf, 1, len, fp);
		}
		fclose(fp);
		return 0;
	}


	if(argc!=2) {
		printf("Usage : %s <port>\n", argv[0]);
		exit(1);
	}	

	// setup ip and port
	strcpy(ip, LISTEN_IP);
	strcpy(port, argv[1]);
	initAddr(&serv_adr, ip, port);


	// ready for listen
	adr_sz = sizeof(serv_adr);
	initSock(&serv_sock, &serv_adr, adr_sz);	
		
	
	// connection admission control
	adr_sz=sizeof(clnt_adr);
	while(1) 
	{
		clnt_sock=accept(serv_sock, (struct sockaddr*)&clnt_adr, &adr_sz);
		if(clnt_sock==-1) {
			continue;
		}
		 else {
			puts("new client connected...");
			clnt_sock_arr[Client_Count++] = clnt_sock;
			Client_Count = Client_Count % 2;
		}
		
	
		pid = fork();
		if(pid == -1) {
			close(clnt_sock);
			continue;
		}

		if(pid == 0){
			close(serv_sock);
			char tmp[100] = "";
			char helloMsg[] = "hello";
			int lenT;
			write(clnt_sock, helloMsg, sizeof(helloMsg));
			lenT = read(clnt_sock, buf, BUF_SIZE);
			buf[lenT] = 0;
			printf("%s %d \n", buf, lenT);
			
			timer = time(NULL);			// 현재 시각을 초 단위로 얻기
			t = localtime(&timer);		// 초 단위의 시간을 분리하여 구조체에 넣기

			sprintf(tmp, "%d.%d.%d %d:%d:%d", 
				t->tm_year + 1900, t->tm_mon + 1, t->tm_mday,
				t->tm_hour, t->tm_min, t->tm_sec);
			
			strcat(tmp, "\t");
			strcat(tmp, inet_ntoa(clnt_adr.sin_addr));
			strcat(tmp, "\t");
			strcat(tmp, buf);
			strcat(tmp, "\n\0");
			write(fds[1], tmp, strlen(tmp));

			while((str_len=read(clnt_sock, buf, BUF_SIZE))!=0) {
				if(clnt_sock == clnt_sock_arr[0]) write(clnt_sock_arr[1], buf, BUF_SIZE);
				else write(clnt_sock_arr[0], buf, BUF_SIZE);
			}
			close(clnt_sock);
			puts("client desconnected...");
			return 0;
		} else {
			close(clnt_sock);
		}
	}
	close(serv_sock);
	return 0;
}

void error_handling(char *message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}


void initAddr(struct sockaddr_in* serv_adr, char* ip, char* port) {
	memset(serv_adr, 0, sizeof(serv_adr));
	serv_adr->sin_family=AF_INET;
	serv_adr->sin_addr.s_addr=inet_addr(ip);
	serv_adr->sin_port=htons(atoi(port));
}


void initSock(int* serv_sock, struct sockaddr_in* serv_adr, int adr_sz) {

	*serv_sock = socket(PF_INET, SOCK_STREAM, 0);   
	if(*serv_sock==-1)
		error_handling("socket() error");	
	
	if(bind(*serv_sock, (struct sockaddr*)serv_adr, adr_sz)==-1)
		error_handling("bind() error");
	
	if(listen(*serv_sock, 5)==-1)
		error_handling("listen() error");
}

void read_childproc(int sig){
	pid_t pid;
	int status;
	pid = waitpid(-1, &status, WNOHANG);
	printf("removed proc id : %d \n", pid);
}

void read_int(int sig){
	flag = 0;
	write(fds[1], "exit", sizeof("exit"));
	sleep(3);
	exit(1);
}
