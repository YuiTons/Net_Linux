#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include <semaphore.h>

#define BUF_SIZE 1024
#define QUES_SIZE 1024
#define NAME_SIZE 10
#define DEPT_SIZE 10
#define ID_SIZE 10

struct QusetionInfo
{
	char name[NAME_SIZE];
	int  stdid;
	char dept[DEPT_SIZE];
	char ques[QUES_SIZE];
	char helpname[NAME_SIZE];
	int  helpstdid;
};

void error_handling(char msg[]);
void* client_thread(void* arg);
void* helper_thread(void* arg);
void* insert_info(void* arg);
void* select_info(void* arg);

pthread_mutex_t fmutex;

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

	pthread_mutex_init(&fmutex, NULL);

	if(bind(serv_sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1)
		error_handling("bind() error");
	if(listen(serv_sock, 5) == -1)
		error_handling("listen() error");


	while(1)
	{
		clnt_adr_size = sizeof(clnt_addr);
		clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_addr, &clnt_adr_size);
		puts("connect");

		strcpy(msg, "사용자를 입력하세요(1.도우미 2.사용자): ");
		write(clnt_sock, msg, strlen(msg));
		read(clnt_sock, &msg, 4);
        	i = atoi(msg);
		printf("select %d\n", i);
      		if(i == 1)
        	{
            		pthread_create(&Helper_t, NULL, helper_thread, (void*)&clnt_sock);
        	}
        	else if(i == 2)
        	{
            		pthread_create(&Client_t, NULL, client_thread, (void*)&clnt_sock);
        	}
		//pthread_detach(???);
	}
	close(serv_sock);

	return 0;
}

void error_handling(char msg[])
{
	puts(msg);
}

void* client_thread(void* arg)
{

}

void* helper_thread(void* arg)
{
    int clnt_sock = *(int*)arg;
    int i, len, recv_len = 0;
    int search_id;
    char msg[BUF_SIZE];
    char Ques[QUES_SIZE];
    char name[NAME_SIZE];
    char *id;
    FILE *Log;

    Log = fopen("log.txt", "a");

    sprintf(msg, "PL도우미 화면입니다\n메뉴를 고르세요\n1. 확인증 등록\n2. 확인증 조회\n");
    write(clnt_sock, msg, strlen(msg));
    read(clnt_sock, &msg, 4);

    switch(atoi(msg))
    {
	    case 1:
		    sprintf(msg, "학번$학과$이름$질문내용$담당자$날짜");
		    write(clnt_sock, msg, strlen(msg));
		    memset(&msg, 0, sizeof(msg));
		    len = read(clnt_sock, &msg, BUF_SIZE);

		    pthread_mutex_lock(&fmutex);
		    fwrite(msg, sizeof(msg), 1, Log);
		    pthread_mutex_unlock(&fmutex);
		    puts("input complete");

		    break;
	    case 2:
		    sprintf(msg, "검색할 학번을 입력하세요");
		    write(clnt_sock, msg, strlen(msg));
		    len = read(clnt_sock, &msg, BUF_SIZE);
		    search_id = atoi(msg);

		    pthread_mutex_lock(&fmutex);
		    while(fgets(msg, BUF_SIZE, Log))
		    {
			    pthread_mutex_unlock(&fmutex);
			    id = strtok(msg, "$");
			    if(atoi(id) == search_id)
				    write(clnt_sock, msg, strlen(msg));
			    pthread_mutex_lock(&fmutex);
		    }
		    pthread_mutex_unlock(&fmutex);

		    write(clnt_sock, "q", 2);

		    sprintf(msg, "검색완료");
		    write(clnt_sock, msg, strlen(msg));

		    break;
    }
    fclose(Log);
}

void* insert_info(void* arg)
{

}

void* select_info(void* arg)
{

}


