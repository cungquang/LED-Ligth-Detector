/*
 * noworky.c
 *
 * This program tries to create two arrays of data, and then swap their
 * contents. However, not all seems to go according to plan...
 */
#include <stdio.h>
#include <stdlib.h>
#include <a2d.h>
#include <stdbool.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include "../include/app_helper.h"
#include "../include/app_sampler.h"
#include "../include/app_upd.h"

#define MAX_LENGTH 500

bool terminate_flag = false;

void trigger_shutdown(int signum){
	if(signum == SIGINT) {
        terminate_flag = true;

		//Set terminate flag to each function
		Sampler_setTerminate(terminate_flag);
		Udp_setTerminate(terminate_flag);
    }
}

int operation(){
	//Register signal handl shutdown
	if(signal(SIGINT, trigger_shutdown) == SIG_ERR) {
		fprintf(stderr, "Error: fail to register signal hanlder\n");
		return 1;
	}
	
	//init & run all slave threads
	init_thread(terminate_flag);

	return 0;
}


int main(int argc, char *argv[])
{
	// if client - no need other thread to be trigger
	if(argc >= 2 && strcmp(argv[1], "CLIENT") == 0)
	{
		printf("%s starting\n", argv[1]);
		init_udpClient();
	} 
	else
	{	
		printf("SERVER starting\n");
		init_udpServer();
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



