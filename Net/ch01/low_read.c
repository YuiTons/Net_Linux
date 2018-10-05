#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#define	BUF_SIZE 100
void error_handling(char *msg);

int main()
{
	int fd;
	char buf[BUF_SIZE];
	
	fd = open("data.txt", O_RDONLY);

	printf("file descriptor: %d\n", fd);

	read(fd, buf, sizeof(buf));

	printf("file data: %s\n", buf);

	close(fd);

	return 0;
}

void error_handling(char *msg)
{
	
}

