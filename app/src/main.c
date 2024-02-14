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
#include "../include/app_helper.h"
#include "../include/app_pthread.h"
#include "../include/app_helper.h"
#include "../include/app_upd.h"

#define MAX_LENGTH 500

bool terminate_flag = false;

void trigger_shutdown(int signum){
	if(signum == SIGINT) {
        terminate_flag = true;
		setTerminate(terminate_flag);
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

	// main process
	while(!terminate_flag)
	{
		//do some logic
		sleepForMs(1000);
		printf("main is sleep");
	}

	return 0;
}


int main(char *args)
{
	if(args == "HOST") {
		
	}

	return 0;
}



