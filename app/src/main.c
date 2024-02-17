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

#define MAX_PRODUCTS 1000

int terminate_flag = 0;

pthread_mutex_t mutex;
sem_t sem_empty;
sem_t sem_full;
int Buffer[MAX_PRODUCTS];
int produce_count = 0;
int copy_count = 0;
static pthread_t write_id;
static pthread_t read_id;

void trigger_shutdown(int signum){
	if(signum == SIGINT) {
        terminate_flag = true;

		//Set terminate flag for all threads
		Udp_cleanup();
		Sampler_cleanup();
    }
}

void *write_toArr()
{
	srand(time(NULL));
	sem_wait(&sem_empty);
	pthread_mutex_lock(&mutex);
	
	Buffer[produce_count++] = rand();
	sleep(500);

	pthread_mutex_unlock(&mutex);
	sem_post(&sem_full);

}

void *read_fromArr()
{
	sem_wait(&sem_full);
	pthread_mutex_lock(&mutex);
	
	Buffer[copy_count++] = rand();

	pthread_mutex_unlock(&mutex);
	sem_post(&sem_empty);
}


//int main(int argc, char *argv[])
int main()
{
	//init mutex & semaphore
	pthread_mutex_init(&mutex, NULL);
    sem_init(&semaphore, 0, 1); 

	//Register signal handle shutdown
	if(signal(SIGINT, trigger_shutdown) == SIG_ERR) {
		fprintf(stderr, "Error: fail to register signal hanlder\n");
		return 1;
	}

	pthread_create(&write_id, NULL, write_toArr, NULL);
	pthread_create(&read_id, NULL, read_fromArr, NULL);


	pthread_join(write_id, NULL);
	pthread_join(read_id, NULL);

	//destroy mutex & semaphore
	pthread_mutex_destroy(&mutex);
    sem_destroy(&semaphore);
	return 0;
}



