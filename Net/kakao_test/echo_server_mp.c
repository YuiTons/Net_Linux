// hello_server.c

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <time.h>
#include <fcntl.h>

#define ListenIP	"127.0.0.1"
#define ListenPort	9876
#define BUF_SIZE 1024

typedef struct sockaddr_in	Addr;

void read_childproc(int sig);
void error_handling(char *msg);
void createSocket(int *listenSock);
void setupListenAddr(Addr *serv_addr, char *port);
void setupBind(int *serv_sock, Addr *serv_addr);
void listenSocket(int *serv_sock);
void accetpConnect(int *serv_sock, int *clnt_sock, Addr *clnt_addr);

int main(int argc, char *argv[]){	
	int listenSock, dataSock;			// 연결요청과 데이터 송수신을 위한 소켓 선언
	struct sockaddr_in  listenAddr, clntAddr;	// 연결요청을 대기할 소켓의 주소값과
							// 클라이언트의 주소값 정보
	int clntAddrSize;				// 클라이언트 주소값의 크기 정보
	int strLen;
	int fd;
	// 전송 데이터
	char message[BUF_SIZE];
	char current_time[20];
	char chatname[10];
	char ipaddr[20];
	char tmp[100] = "";
	time_t timer;
	struct tm *t;
	pid_t pid;
	struct sigaction act;
	int state;

	int fds[2];

	
	act.sa_handler = read_childproc;
	sigemptyset(&act.sa_mask);
	act.sa_flags = 0;
	state = sigaction(SIGCHLD, &act, 0);

	fd = open("log.txt", O_APPEND | O_WRONLY);

	// 1) 소켓을 생성하는 함수 호출
	createSocket(&listenSock);
	// 2) 연결요청을 대기 할 주소 설정
	setupListenAddr(&listenAddr, argv[1]);
	// 3) 소켓에 주소 설정
	setupBind(&listenSock, &listenAddr);
	// 4) 연결요청 대기를 위한 함수 호출
	listenSocket(&listenSock);
	
	while(1){
		// 5) 연결요청에 대한 수락 여부 결정
		accetpConnect(&listenSock, &dataSock, &clntAddr);
		pid = fork();
		if(pid == 0) //child
		{
			close(listenSock);
			read(dataSock, chatname, 10);
			timer = time(NULL);
			t = localtime(&timer);
			sprintf(current_time, "%04d-%02d-%02d %02d:%02d:%02d ", t->tm_year+1900, 
					t->tm_mon+1, t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec);
			strcat(tmp, current_time);
			strcat(tmp, inet_ntoa(clntAddr.sin_addr));
			strcat(tmp, " ");
			strcat(tmp, chatname);
			write(fd, tmp, strlen(tmp));

			while((strLen=read(dataSock, message, BUF_SIZE))!=0)
				write(dataSock, message, strLen);
			close(dataSock);
			return 0;
		}
		else	    //parents
		{
			close(dataSock);
		}

	}

	close(listenSock);

	return 0;
}

void read_childproc(int sig) {
	pid_t pid;
	int status;
	pid=waitpid(-1, &status, WNOHANG);
	printf("removed proc id: %d\n", pid);
}

void error_handling(char *msg) {
	fputs(msg, stderr);
	fputc('\n', stderr);
	exit(1);
}

void createSocket(int *listenSock) {
	*listenSock = socket(PF_INET, SOCK_STREAM, 0);
	if(*listenSock == -1) {
		error_handling("socket() error");
	}
}

void setupListenAddr(Addr *serv_addr, char *port){
	memset(serv_addr, 0, sizeof(Addr));
	serv_addr->sin_family = AF_INET;
	serv_addr->sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr->sin_port = htons( atoi(port) );
}

void setupBind(int *serv_sock, Addr *serv_addr){
	int errM;
	errM = bind(*serv_sock, (struct sockaddr*)serv_addr, sizeof(Addr));
	if(errM == -1) 
		error_handling("bind() error");	
}

void listenSocket(int *serv_sock) {
	int errM;
	errM = listen(*serv_sock, 5);
	if(errM == -1)
		error_handling("listen() error");
}

void accetpConnect(int *serv_sock, int *clnt_sock, Addr *clnt_addr) {
	int clnt_addr_size;
	clnt_addr_size = sizeof(Addr);
	*clnt_sock = accept(*serv_sock, (struct sockaddr*)clnt_addr, &clnt_addr_size);
	if(*clnt_sock == -1)
		error_handling("accept() error");
}
