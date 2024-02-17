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

int terminate_flag = 0;

void trigger_shutdown(int signum){
	if(signum == SIGINT) {
        terminate_flag = true;

		//Set terminate flag for all threads
		Udp_cleanup();
		Sampler_cleanup();
    }
}


//int main(int argc, char *argv[])
int main()
{
	//Register signal handle shutdown
	// if(signal(SIGINT, trigger_shutdown) == SIG_ERR) {
	// 	fprintf(stderr, "Error: fail to register signal hanlder\n");
	// 	return 1;
	// }

	//operation();
	
	printf("%s", command_help());

	return 0;
}



