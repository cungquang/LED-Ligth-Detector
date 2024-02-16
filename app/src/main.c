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

static bool terminate_flag = false;

void trigger_shutdown(int signum){
	if(signum == SIGINT) {
        terminate_flag = true;

		//Set terminate flag for all threads
		Sampler_setTerminate(terminate_flag);
		Udp_setTerminate(terminate_flag);
    }
}

//int main(int argc, char *argv[])
int main()
{
	//Register signal handl shutdown
	if(signal(SIGINT, trigger_shutdown) == SIG_ERR) {
		fprintf(stderr, "Error: fail to register signal hanlder\n");
		return 1;
	}

	//Initiate all programs
	Sampler_init(terminate_flag);
	Udp_initServer(terminate_flag);

	//Join
	Udp_cleanup();
	Sampler_cleanup();
	
	return 0;
}



