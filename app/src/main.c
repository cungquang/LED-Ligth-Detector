#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <signal.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include "../include/app_helper.h"
#include "../include/app_sampler.h"
#include "../include/app_upd.h"

bool terminate_flag = false;
static pthread_t programShutdown_id;

void trigger_shutdown(int signum){
	if(signum == SIGINT) {
        terminate_flag = true;

		//Set terminate flag for all threads
		Sampler_setTerminate(terminate_flag);
		Udp_setTerminate(terminate_flag);
    }
}

void *Program_shutdown()
{
	while(!terminate_flag)
	{
		sleepForMs(1001);
	}

	//cleanup UDP
	Udp_cleanup();
	Sampler_cleanup();
	return NULL;
}

int operation(){
	//Register signal handl shutdown
	if(signal(SIGINT, trigger_shutdown) == SIG_ERR) {
		fprintf(stderr, "Error: fail to register signal hanlder\n");
		return 1;
	}
	
	//init & run all slave threads
	Sampler_init(terminate_flag);

	return 0;
}


int network(int argc, char *argv[])
{
	// if client - no need other thread to be trigger
	if(argc >= 2 && strcmp(argv[1], "CLIENT") == 0)
	{
		printf("%s starting\n", argv[1]);
		Udp_initClient(terminate_flag);
	} 
	else
	{	
		printf("SERVER starting\n");
		Udp_initServer(terminate_flag);
	}

	return 0;
}

//int main(int argc, char *argv[])
int main()
{
	//Start thread Program_shutdown thread
	if(pthread_create(&programShutdown_id, NULL, Program_shutdown, NULL) != 0){
        return 1;
    }

	// main process - keep the prgram runing
	while(!terminate_flag)
	{
		//do some logic
		sleepForMs(1001);
		printf("main is sleep");
	}

	return 0;
}



