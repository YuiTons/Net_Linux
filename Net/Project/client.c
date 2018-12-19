#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>
#include <netinet/in.h>
#include <semaphore.h>

#define BUF_SIZE 1024
#define QUES_SIZE 1024
#define NAME_SIZE 10
#define DEPT_SIZE 10
#define ID_SIZE 10

struct QuestionsInfo
{
};

void error_handling(char msg[]);

int main(int argc, char* argv[])
{
	int sock;
	struct sockaddr_in sock_addr;
	int i, j, k;
	char msg[BUF_SIZE];
	int len = 0;

	if(argc!=3)
	{
		printf("Usage: %s <ip> <port>\n", argv[0]);
		exit(1);
	}

	sock = socket(PF_INET, SOCK_STREAM, 0);

	memset(&sock_addr, 0, sizeof(sock_addr));
	sock_addr.sin_family = AF_INET;
	sock_addr.sin_addr.s_addr = inet_addr(argv[1]);
	sock_addr.sin_port = htons(atoi(argv[2]));

	if(connect(sock, (struct sockaddr*)&sock_addr, sizeof(sock_addr)) == -1)
		error_handling("connect() error");

	read(sock, &msg, BUF_SIZE);
	puts(msg);
	while(1)
	{
		scanf("%s", msg);
		write(sock, msg, strlen(msg));
		if(strcmp(msg, "1") == 0)
		{
			//질문자 선택
			read(sock, &msg, BUF_SIZE);
			puts(msg);

			while(1)
			{
				scanf("%s", msg);
				write(sock, msg, strlen(msg));
				int checknum = atoi(msg);
				if(checknum == 1)
				{
					len = read(sock, &msg, BUF_SIZE);
					msg[len] = '\0';
					puts(msg);
					scanf("%s", msg);
					sprintf(msg, "%ld%s", strlen(msg), msg);

				}
				else if(checknum == 2)
				{
				}
				else
				{
					puts("잘못된 입력입니다");
					continue;
				}
			}

			break;
		}
		else if(strcmp(msg, "2") == 0)
		{
			//도우미 선택
			break;
		}
		else
		{
			puts("잘못된 입력입니다");
			continue;
		}
	}

	for(i=0;i<len;i++)
	{
		read(sock, &msg, BUF_SIZE);
		printf("%s", msg); fflush(stdout);
	}
	return 0;
}

void error_handling(char msg[])
{
	puts(msg);
	exit(1);
}
