#define REENTRANT
#include <stdio.h>
#include <pthread.h>

int sum = 0;
int num = 0;

void *thread_summation(void *arg)
{
	int start = ((int*)arg)[0];
	int end	  = ((int*)arg)[1];

	while(start <= end)
	{
		sum += start;
		start++;
	}

	return NULL;
}

void *thread_inc(void *arg)
{
	int i;
	for(i = 0; i < 50000000; i++)
		num+=1;
	return NULL;
}

void *thread_des(void *arg)
{
	int  i;
	for(i = 0; i< 50000000; i++)
		num -= 1;
	return NULL;
}

int main(int argc, char *argv[])
{
	pthread_t id_t1, id_t2;
	int range1[] = {1, 50};
	int range2[] = {51, 100};
	pthread_mutex_init(&mutex, NULL);

	pthread_create(&id_t1, NULL, thread_inc, NULL);
	pthread_create(&id_t2, NULL, thread_des, NULL);

	pthread_join(id_t1, NULL);
	pthread_join(id_t2, NULL);
	printf("result: %d \n", num);
	return 0;
}
