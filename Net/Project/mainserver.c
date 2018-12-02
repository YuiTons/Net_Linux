#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>

#define BUF_SIZE 1024
#define QUES_SIZE 1024
#define NAME_SIZE 10
#define DEPT_SIZE 10
#define ID_SIZE 10

struct QusetionInfo
{

};

void* client_thread(void* arg);
void* helper_thread(void* arg);
void* insert_info(void* arg);
void* select_info(void* arg);

int main(int argc, char* argv[])
{
	int serv_sock, clnt_sock;
	struct sockaddr_in serv_addr, clnt_addr;
	int clnt_adr_size;
	int i, j, k;
	pthread_t Client_t, Helper_t;
	char msg[BUF_SIZE];

	if(argc!=2)
	{
		printf("Usage: %s <port>\n", argv[0]);
		exit(1);
	}

	serv_sock = socket(PF_INET, SOCK_STREAM, 0);

	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = htons(atoi(argv[1]));
	
	if(bind(serv_sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1)
		error_handling("bind() error");
	if(listen(serv_sock, 5) == -1)
		error_handling("listen() error");

	while(1)
	{
		clnt_adr_size = sizeof(clnt_addr);
		clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_addr, &clnt_addr_size);

		strcpy(msg, "사용자를 입력하세요(1.도우미 2.사용자): ");
		write(clnt_sock, msg, strlen(msg));
		read(clnt_sock, &msg, 4);

	}
	close(serv_sock);
	close(clnt_sock);

	return 0;
}

void* client_thread(void* arg);
{

}

void* helper_thread(void* arg);
{

}

void* insert_info(void* arg);
{

}

void* select_info(void* arg);
{

}


