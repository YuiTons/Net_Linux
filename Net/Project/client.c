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
			//도우미 선택
			read(sock, &len, 4);
			read(sock, &msg, len);	//1,등록
			msg[len] = '\0';
			puts(msg);

			while(1)
			{
				len = 0;
				scanf("%s", msg);
				write(sock, msg, strlen(msg));
				int checknum = atoi(msg);
				if(checknum == 1) //등록
				{
					memset(&msg, 0, sizeof(msg));
					read(sock, &len, 4);
					read(sock, &msg, len);
					puts(msg);
					scanf("%s", msg);
					int length = strlen(msg);
					write(sock, &length, 4);
					write(sock, msg, length);
					read(sock, &len, 4);
					read(sock, &msg, len);
					msg[len] = '\0';
					puts(msg);
					break;
				}
				else if(checknum == 2) //조회
				{
					int flag = 0;
					read(sock, &len, 4);
					read(sock, &msg, len);
					msg[len] = '\0';
					puts(msg);
					scanf("%s", msg);

					len = strlen(msg);
					write(sock, &len, 4);
					write(sock, msg, len);

					read(sock, &flag, 4);
					if(flag)
					{
						read(sock, &len, 4);
						read(sock, msg, len);
						msg[len] = '\0';
						puts(msg);
					}
					else
					{
						puts("검색 결과 해당 학번은 등록된 기록이 없습니다");
					}
					break;
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
			//질문자 선택
			int flag = 0;
			puts("질문자 검색");
			read(sock, &len, 4);
			read(sock, &msg, len);
			msg[len] = '\0';
			puts(msg);
			scanf("%s", msg);

			len = strlen(msg);
			write(sock, &len, 4);
			write(sock, msg, len);

			read(sock, &flag, 4);
			if(flag)
			{
				read(sock, &len, 4);
				read(sock, msg, len);
				msg[len] = '\0';
				puts(msg);
			}
			else
			{
				puts("검색 결과 해당 학번은 등록된 기록이 없습니다");
			}
			break;
		}
		else
		{
			puts("잘못된 입력입니다");
			continue;
		}
	}

	puts("프로그램을 종료합니다");
	return 0;
}

void error_handling(char msg[])
{
	puts(msg);
	exit(1);
}
