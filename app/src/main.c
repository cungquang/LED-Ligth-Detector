#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <signal.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <semaphore.h>
#include "../include/app_sampler.h"
#include "../include/app_upd.h"
#include "../include/app_helper.h"

#define MAX_PRODUCTS 1000

int terminate_flag = 99;

pthread_mutex_t mutex;
sem_t sem_full;
sem_t sem_empty;

int Buffer[MAX_PRODUCTS];
int produce_count = 0;

int *Buffer_cp = NULL;
int copy_count = 0;

static pthread_t write_id;
static pthread_t read_id;

void *write_toArr()
{
	//Wait until the sem_empty > 0 -> decrement it -> start operation
	sem_wait(&sem_empty);
	//mutex protect the shared resource
	pthread_mutex_lock(&mutex);
	
	for(int i = 0; i < 10; i++){
		Buffer[produce_count++] = i;
	}

	pthread_mutex_unlock(&mutex);

	//increment sem_full -> indicate the shared resource is available
	sem_post(&sem_full);
	return NULL;
}

void *read_fromArr()
{
	//Wait until the sem_full > 0 -> decrement it -> start operation
	sem_wait(&sem_full);

	//mutex protect the shared resource
	pthread_mutex_lock(&mutex);
	
	//free previous array
	if(copy_count < produce_count)
	{
		copy_count = produce_count;
		if(Buffer_cp)
		{
			free(Buffer_cp);
			Buffer_cp = NULL;
		}
		Buffer_cp = (int *) malloc((produce_count) * sizeof(int));
	}

	for(int i = 0; i < produce_count; i++)
	{
		Buffer_cp[i] = Buffer[i];
	}

	pthread_mutex_unlock(&mutex);

	//after copy all element -> Buffer should be empty -> increase sem_empty -> allow to write
	sem_post(&sem_empty);
	
	return NULL;
}

void testMutexSemaphore(void) 
{
	//init mutex & semaphore
	pthread_mutex_init(&mutex, NULL);

	//1st para: address to semaphore
	//2nd para: number of process sharing this semaphore
	//3rd para: initial value
	sem_init(&sem_empty, 0, 1);
    sem_init(&sem_full, 0, 0); 

	pthread_create(&write_id, NULL, write_toArr, NULL);
	pthread_create(&read_id, NULL, read_fromArr, NULL);


	pthread_join(write_id, NULL);
	pthread_join(read_id, NULL);

	//print both Buffer & Buffer_cp
	printf("Raw %d\n", produce_count);
	for(int i = 0; i < produce_count; i++)
	{
		printf("%d-", Buffer[i]);
	}
	printf("\n");

	printf("Raw %d\n", copy_count);
	for(int i = 0; i < copy_count; i++)
	{
		printf("%d-", Buffer_cp[i]);
	}
	printf("\n");

	//destroy mutex & semaphore
	pthread_mutex_destroy(&mutex);
    sem_destroy(&sem_full);
	sem_destroy(&sem_empty);

	if(Buffer_cp)
	{
		free(Buffer_cp);
		Buffer_cp = NULL;
	}
}

void setInteger(int *size)
{
	*size = terminate_flag;
}

//int main(int argc, char *argv[])
int main()
{	
	int batch_size = 0;
	printf("before - %d", batch_size);
	setInteger(&batch_size);
	printf("after - %d", batch_size);

	return 0;
}



