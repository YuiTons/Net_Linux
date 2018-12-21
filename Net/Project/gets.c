#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main()
{
	FILE *f = fopen("log.txt", "r+");
	char msg[100];
	fgets(msg, 100, f);
	puts(msg);
	return 0;
}
