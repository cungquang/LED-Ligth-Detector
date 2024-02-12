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


/*
 * Create two arrays; populate them; swap them; display them.
 */
int main()
{
	bool isStart = true;

	//Register signal handl shutdown
	if(signal(SIGINT, handle_shutdown) == SIG_ERR) {
		fprintf(stderr, "Error: fail to register signal hanlder\n");
		return 1;
	}

	//init & run all slave threads
	init_thread(isStart);

	// main process
	while(isStart)
	{
		//do some logic
		printf("main is sleep");
		sleepForMs(1);
	}

	return 0;
}

