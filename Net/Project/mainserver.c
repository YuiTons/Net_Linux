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
	int f1 = 0, f2 = 0;

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
			f1 = 1;
        	}
        	else if(i == 2)
        	{
            		pthread_create(&Client_t, NULL, client_thread, (void*)&clnt_sock);
			f2 = 1;
        	}

		if(f1)
		{
			pthread_detach(Helper_t);
			f1 = 0;
		}
		if(f2)
		{
			pthread_detach(Client_t);
			f2 = 0;
		}
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
    int clnt_sock = *(int*)arg;
    int i, len, recv_len = 0;
    int search_id;
    char msg[BUF_SIZE];
    char Ques[QUES_SIZE];
    char name[NAME_SIZE];
    char *id;
    FILE *Log;
    int flag = 0;
    char msg2[BUF_SIZE];
    char msg3[BUF_SIZE];
    Log = fopen("log.txt", "r+");

    sprintf(msg, "검색할 학번을 입력하세요");
    len = strlen(msg);
    write(clnt_sock, &len, 4);
    write(clnt_sock, msg, len);
    puts("write");
    read(clnt_sock, &len, 4);
    read(clnt_sock, &msg, len);
    puts("read");
    search_id = atoi(msg);
    printf("id: %d\n", search_id);
    memset(&msg, 0, sizeof(msg));

    pthread_mutex_lock(&fmutex);
    while(fgets(msg2, BUF_SIZE, Log)!=NULL)
    {
	    pthread_mutex_unlock(&fmutex);
	    strcpy(msg3, msg2);
	    id = strtok(msg2, "$");
	    if(atoi(id) == search_id)
	    {
		    flag++;
		    strcat(msg, msg3);
	    }

	    pthread_mutex_lock(&fmutex);
    }
    pthread_mutex_unlock(&fmutex);
    len = strlen(msg);

    write(clnt_sock, &flag, 4);
    write(clnt_sock, &len, 4);
    write(clnt_sock, msg, len);
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
    int flag = 0;
    char msg2[BUF_SIZE];
    char msg3[BUF_SIZE];
    Log = fopen("log.txt", "r+");

    memset(&msg, 0, strlen(msg));
    sprintf(msg, "PL도우미 화면입니다\n메뉴를 고르세요\n1. 확인증 등록\n2. 확인증 조회\n");
    len = strlen(msg);
    write(clnt_sock, &len, 4);
    write(clnt_sock, msg, len);
    read(clnt_sock, &msg, 4);

    switch(atoi(msg))
    {
	    case 1:
		    memset(&msg, 0, sizeof(msg));
		    sprintf(msg, "학번$학과$이름$질문내용$담당자$날짜");
		    len = strlen(msg);
		    write(clnt_sock, &len, 4);
		    write(clnt_sock, msg, len);
		    memset(&msg, 0, sizeof(msg));
		    read(clnt_sock, &len, 4);
		    read(clnt_sock, &msg, len);
		    msg[len] = '\n';
		    pthread_mutex_lock(&fmutex);
		    fseek(Log, 0, SEEK_END);
		    fwrite(msg, strlen(msg), 1, Log);
		    pthread_mutex_unlock(&fmutex);
		    sprintf(msg, "%s input complete", msg);
		    puts(msg);
		    len = strlen(msg);
		    write(clnt_sock, &len, 4);
		    write(clnt_sock, msg, len);

		    break;
	    case 2:
		    sprintf(msg, "검색할 학번을 입력하세요");
		    len = strlen(msg);
		    write(clnt_sock, &len, 4);
		    write(clnt_sock, msg, len);
		    read(clnt_sock, &len, 4);
		    read(clnt_sock, &msg, len);
		    search_id = atoi(msg);
		    printf("id: %d\n", search_id);
		    memset(&msg, 0, sizeof(msg));

		    pthread_mutex_lock(&fmutex);
		    while(fgets(msg2, BUF_SIZE, Log)!=NULL)
		    {
			    pthread_mutex_unlock(&fmutex);
			    strcpy(msg3, msg2);
			    id = strtok(msg2, "$");
			    if(atoi(id) == search_id)
			    {
				    flag++;
				    strcat(msg, msg3);
			    }

			    pthread_mutex_lock(&fmutex);
		    }
		    pthread_mutex_unlock(&fmutex);
		    len = strlen(msg);

		    write(clnt_sock, &flag, 4);
		    write(clnt_sock, &len, 4);
		    write(clnt_sock, msg, len);

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


