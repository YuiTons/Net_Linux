#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

void error_handling(char *meg);

int main(int arpc, char *argv[])
{
	int serv_sock, clnt_sock;
	int fd;
	int len = 0;
	struct sockaddr_in serv_addr;
	struct sockaddr_in clnt_addr;
	socklen_t clnt_addr_size;
	char message[100];

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

	clnt_addr_size = sizeof(clnt_addr);
	clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_addr, &clnt_addr_size); //Create Data Socket
	if(clnt_sock == 1)
		error_handling("bind() error");

	fd = open("data1.txt", O_RDONLY);

	while(1)
	{
		len = read(fd, message, 99);
		if(len == 0) { printf("aaa");	break;}
		write(clnt_sock, message, len);
	}

	close(fd);
	close(clnt_sock);
	close(serv_sock);
	return 0;

}

void error_handling(char *meg)
{
	fputs(meg, stderr);
	fputs('\n', stderr);
	exit(1);
}
