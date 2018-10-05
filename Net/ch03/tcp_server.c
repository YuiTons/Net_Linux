#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#include<unistd.h>
#include<arpa/inet.h>
#include<sys/socket.h>

#define ListenIP "127.0.0.1"
#define ListenPort 9876

typedef struct sockaddr_in Addr;

void error_handling(char *msg);
void createSocket(int *listenSock);
void setupListenAddr(Addr *serv_Addr, char *port);
void setupBind(int *serv_sock, Addr *serv_addr);
void acceptConnect(int *serv_sock, int *dataSock, Addr *clntAddr, int *clntAddrSize);
void listenSocket(int *serv_sock);

int main(int argc,char*argv[]){
	int listenSock, dataSock;	// 연결요청과 데이터 송수신을 위한 소켓 
	Addr listenAddr, clntAddr;	// 연결 요청을 대기할 소켓
					// 클라이언트의 주소값 정보	

	int clntAddrSize; // 클라이언트 주소값의 크기

	char message[] = "Hello World!"; // 전송 데이터

	// 1) 소켓 호출 함수
	createSocket(&listenSock);
	// 2) 연결요청을 대기 할 주소 설정
	setupListenAddr(&listenAddr, argv[1]);
	// 3) 소켓에 주소 설정
	setupBind(&listenSock, &listenAddr);
	// 4) 연결요청 대기
	listenSocket(&listenSock);
	// 5) 연결요청에 대한 수락여부 결정
	acceptConnect(&listenSock, &dataSock, &clntAddr, &clntAddrSize);

	// 6) 데이터 송수신
	write(dataSock, message, sizeof(message));


	close(dataSock);
	close(listenSock);

	return 0;
}

void error_handling(char *msg){
	fputs(msg, stderr);
	fputs("\n", stderr);
	exit(1);
}

void createSocket(int *listenSock){
	*listenSock = socket(PF_INET,SOCK_STREAM, 0);
	if(*listenSock == -1){
		error_handling("socket() error");
	}
}

void setupListenAddr(Addr *serv_addr, char *port){
	memset(serv_addr,0,sizeof(*serv_addr));
	serv_addr->sin_family = AF_INET;
	serv_addr->sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr->sin_addr.s_addr = inet_addr(ListenIP);
	serv_addr->sin_port = htons(atoi(port));
}

void setupBind(int *serv_sock, Addr *serv_addr){
	if(bind(*serv_sock,(struct sockaddr*)serv_addr,sizeof(*serv_addr))==-1){
		error_handling("bind() error");
	}
}

void listenSocket(int *serv_sock){
	if(listen(*serv_sock, 5)==-1)
		error_handling("listen() error");

}

void acceptConnect(int *serv_sock, int *dataSock, Addr *clntAddr, int *clntAddrSize){
	*clntAddrSize = sizeof(*clntAddr);
	*dataSock = accept(*serv_sock, (struct sockaddr*)clntAddr, clntAddrSize);
	if(*dataSock == -1)	error_handling("accept() error!");
}
