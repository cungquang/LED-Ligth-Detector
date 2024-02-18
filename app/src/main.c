#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>
#include "../include/app_sampler.h"
#include "../include/app_upd.h"
#include "../include/app_helper.h"

int terminate_flag;

int main()
{	
	terminate_flag = 0;
	Udp_initServer(&terminate_flag);

	Udp_join();

	Udp_cleanup();
	return 0;
}
