#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

void error_handling(char *msg);


int main(int argc, char *argv[])
{
	int sock;
	struct sockaddr_in serv_addr;
	int len, idx=0,str_len=0, read_len = 0;
	char msg[100];

	sock = socket(PF_INET, SOCK_STREAM, 0); //Create Data Socket
	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
	serv_addr.sin_port = htons(atoi(argv[2]));

	connect(sock,(struct sockaddr*)&serv_addr, sizeof(serv_addr));

	//len = read(sock, msg, sizeof(msg)-1);
	while(read_len = read(sock, &msg[idx++], 1))
	{
		if(read_len == -1)	error_handling("");
		str_len += read_len;
	}

	printf("Message from server: %s \n",msg);
	close(sock);
	
	return 0;	
}

void error_handling(char *msg)
{
	fputs(msg, stderr);
	fputc('\n', stderr);
	exit(1);
}
