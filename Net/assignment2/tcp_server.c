// hello_server.c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define ListenIP	"127.0.0.1"
#define ListenPort	9876
#define OPSZ 4
#define BUF_SIZE 1024

typedef struct sockaddr_in	Addr;

void error_handling(char *msg);
void createSocket(int *listenSock);
void setupListenAddr(Addr *serv_addr, char *port);
void setupBind(int *serv_sock, Addr *serv_addr);
void listenSocket(int *serv_sock);
void accetpConnect(int *serv_sock, int *clnt_sock, Addr *clnt_addr);
int calculate(int opnum, int *opnds, char op);

int main(int argc, char *argv[]){	
	int listenSock, dataSock;	// 연결요청과 데이터 송수신을 위한 소켓 선언
	struct sockaddr_in  listenAddr, clntAddr;	// 연결요청을 대기할 소켓의 주소값과
												// 클라이언트의 주소값 정보
	int clntAddrSize;			// 클라이언트 주소값의 크기 정보
	int opnd_cnt, recv_len, recv_cnt, i, result;
	char opinfo[BUF_SIZE];

	// 전송 데이터
	char message[] = "Hello World!";

	// 1) 소켓을 생성하는 함수 호출
	createSocket(&listenSock);
	// 2) 연결요청을 대기 할 주소 설정
	setupListenAddr(&listenAddr, argv[1]);
	// 3) 소켓에 주소 설정
	setupBind(&listenSock, &listenAddr);
	// 4) 연결요청 대기를 위한 함수 호출
	listenSocket(&listenSock);
	accetpConnect(&listenSock, &dataSock, &clntAddr);
	for(i=0;i<3;i++)
	// 5) 연결요청에 대한 수락 여부 결정
	{	
		opnd_cnt = 0;
		
		read(dataSock, &opnd_cnt, 4);
		recv_len = 0;
		*(int*)&opinfo = opnd_cnt;

		while(opnd_cnt > recv_len )
		{
			recv_cnt = read(dataSock, &opinfo[recv_len + 4], 1);
			if(recv_cnt == -1) error_handling("recv read() error");

			recv_len += recv_cnt;
		}
		opinfo[recv_len + 4] = '\0';
		printf("client: %s \n", &opinfo[4]);

		write(dataSock, opinfo, opnd_cnt + 4);
	}
	close(dataSock);
	close(listenSock);

	return 0;
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

int calculate(int opnum, int *opnds, char op)
{
	int result = opnds[0];
	int i;
	
	switch(op){
	case '+':
		for(i=1;i<opnum;i++) result+=opnds[i]; break;
	case '-':
		for(i=1;i<opnum;i++) result-=opnds[i]; break;
	case '*':
		for(i=1;i<opnum;i++) result*=opnds[i]; break;
	}
	return result;
}
