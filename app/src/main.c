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
		Sampler_setTerminate(terminate_flag);
    }
}

void operation()
{
	//Initiate all programs
	Sampler_init(terminate_flag);
	Udp_initServer(&terminate_flag);

	//Join
	Udp_cleanup();
	Sampler_cleanup();
}

//int main(int argc, char *argv[])
int main()
{
	//Register signal handl shutdown
	// if(signal(SIGINT, trigger_shutdown) == SIG_ERR) {
	// 	fprintf(stderr, "Error: fail to register signal hanlder\n");
	// 	return 1;
	// }
	Udp_setTerminate(&terminate_flag);
	
	printf("before %d\n", terminate_flag);

	command_stop();
	printf("after %d\n", terminate_flag);

	
	return 0;
}



