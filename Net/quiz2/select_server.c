// 메시지가 들어온 경우 무슨 내용인지 확인해서 다르게 처리해야 함
//절차식으로 할 경우 두개 이상의 소켓이 동시에 보낼경우 문제가 생김
//전송하는 메시지 앞에 대화명/답안 등을 구분하기 위한 서비스코드 or 단어를 붙여서 전송
//받은 데이터 앞부분으로 무슨 내용인지 파악하고 이후 처리
//서비스코드 만큼은 무조건 읽도록 (read를 1바이트씩 4번 반복 등) 하고 이후 데이터를 받음
//서비스코드와 파일크기를 담아서 전송, 받는쪽은 서비스코드와 파일크기를 우선적으로 받아서 이후 데이터처리
//클라이언트는 서버와 1:1로 연결되는 방식이므로 절차식으로 구성해도 됨

#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <time.h>


void error_handling(char *meg);

#define HELLO_SIZE 6
#define NICKNAME_SIZE 10
#define WORD_SIZE 7
#define EX_SIZE 12 
#define TIME_SIZE 255
#define RANK_SIZE 5

struct User{
	int time;
	int correct;
	int count;
	int sock;
	int rank;
	char answer[3][WORD_SIZE];
	char name[10];
};

void setstruct(struct User *rank)
{
	rank->time = 0;
	rank->correct = 0;
	rank->count = 0;
	rank->sock = -1;
	rank->rank = -1;
}

void copystruct(struct User *rank, struct User *tmp, int num)
{
	int i, len;
	for(i=0;i<num-1;i++)
	{
		rank->count;
	}
}

int main(int arpc, char *argv[])
{
	int serv_sock, clnt_sock;
	struct sockaddr_in serv_addr;
	struct sockaddr_in clnt_addr;
	struct timeval timeout;
	fd_set reads, cpy_reads;
	struct User *ranking, *ranktmp, *ranks3;
	struct User tmprank;

	socklen_t clnt_addr_size;
	int fd_max, str_len, fd_num, i, b, l, n, m;
	int word_len, comma_index, user_num = 0, rankers = 0;
	int timemin = 0;
	char username[256][NICKNAME_SIZE] = {""};
	char buf[1024];
	char tmp[255];
	char words[3][WORD_SIZE] = { "apple", "orange", "peach" };
	char blank[3][WORD_SIZE];
	char exs[3][EX_SIZE] = { "red fruit", "yello fruit", "pink fruit" };
	
	srand((unsigned int)time(NULL));

	serv_sock = socket(PF_INET, SOCK_STREAM, 0); //Create Listen Socket
	if(serv_sock == -1) error_handling("socket() error");
	
	memset(&serv_addr, 0 ,sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr =  htonl(INADDR_ANY);
	serv_addr.sin_port = htons(atoi(argv[1]));

	if(bind(serv_sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1)
		error_handling("bind() error");

	if(listen(serv_sock, 5) == -1)
		error_handling("listen() error");

	FD_ZERO(&reads);
	FD_SET(serv_sock, &reads);
	fd_max = serv_sock;

	while(1)
	{
		cpy_reads = reads;
		timeout.tv_sec = 5;
		timeout.tv_usec = 5000;
		
		if((fd_num = select(fd_max+1, &cpy_reads, 0, 0, &timeout)) == -1)
			break;
		if(fd_num == 0)
			continue;

		for(i=0; i<fd_max+1; i++)
		{
			if(FD_ISSET(i, &cpy_reads))
			{
				if(i==serv_sock)
				{
					clnt_addr_size = sizeof(clnt_addr);
					clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_addr, &clnt_addr_size);
					FD_SET(clnt_sock, &reads);
					if(fd_max < clnt_sock) fd_max = clnt_sock;
					//printf("connected client: %d \n", clnt_sock);
					write(i, "hello", HELLO_SIZE);
					read(i, buf, NICKNAME_SIZE);

					if(!user_num) ranking = (struct User*)malloc(sizeof(struct User)*(++user_num));
					else
					{
						ranktmp = ranking;
						ranking = (struct User *)malloc(sizeof(struct User)*(++user_num));
						//copystruct(ranking, ranktmp);	
						for(b=0;b<user_num-1;b++)
							ranking[b]=ranktmp[b];
						free(ranktmp);
						setstruct(&ranking[b]);
					}

					strcpy(ranking[user_num - 1].name, buf);
					strcpy(username[i], buf);
					
					for(b=0;b<3;b++)
					{
						strcpy(blank[b], words[b]);
						word_len = strlen(blank[b]);
						blank[b][rand()%word_len] = '*';
						strcpy(buf, blank[b]);
						strcat(buf, ", ");
						strcat(buf, exs[b]);
						write(i, buf, WORD_SIZE + EX_SIZE + 2);
					}
				}
				else
				{
					str_len = read(i, buf, WORD_SIZE + TIME_SIZE + 2);
					if(str_len == 0)
					{
						FD_CLR(i, &reads);
						close(i);
						printf("closed client: %d \n", i);
					}
					else
					{
						//input answer&time
						word_len = strlen(buf);
						for(l=0;l<word_len;l++)
						{
							if(buf[l] == ',')
							{
								comma_index = l;
								break;
							}
						}

						for(l=0;l<user_num;l++)
						{
							if(strcmp(ranking[l].name, username[i]) == 0) break;
						}
						
						for(b=0;b<comma_index;b++)
							ranking[l].answer[ranking[l].count][b] = buf[b];
						ranking[l].answer[ranking[l].count][b] = '\0';

						if(strcmp(ranking[l].answer[ranking[l].count], words[ranking[l].count]) == 0)
							ranking[l].correct++;

						for(l=comma_index+1; buf[l]; l++) 
							tmp[l-comma_index+1] = buf[l];

						tmp[l] = '\0';
						ranking[l].time += atoi(tmp);
						
						if(ranking[l].correct == 3)
						{
							if(rankers>0)
							{
								ranktmp = ranks3;
								ranks3 = (struct User*)malloc(sizeof(struct User)*(++rankers));
								for(b=0;b<rankers-1;b++);
									ranks3[b]=ranktmp[b];
								ranks3[b]=ranking[l];
								free(ranktmp);
							}
							else
							{
								ranks3 = (struct User*)malloc(sizeof(struct User)*(++rankers));
								ranks3[0] = ranking[l];
							}
							for(n=0;n<rankers;n++)
							{
								for(m=n+1;m<rankers;m++)
								{
									if(ranks3[m].time < ranks3[timemin].time) timemin = m;	
								}
								tmprank = ranks3[n];
								ranks3[n]=ranks3[timemin];
								ranks3[timemin]=tmprank;
							}
							for(n=0;n<rankers;n++)
								if(strcmp(ranks3[n].name, username[i]) == 0)
								{
									sprintf(buf, "%s", (char*)n);
									write(i, buf, RANK_SIZE);
								}
						}
					}
				}
			}
		}
	}

	close(clnt_sock);
	close(serv_sock);
	free(ranks3);
	free(ranking);
	return 0;
}

void error_handling(char *meg)
{
	fputs(meg, stderr);
	fputs("\n", stderr);
	exit(1);
}
