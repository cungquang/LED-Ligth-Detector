#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>
#include "../include/app_sampler.h"
#include "../include/app_upd.h"
#include "../include/app_helper.h"
#include "../include/app_ledP921.h"

int terminate_flag;

void operation()
{
	terminate_flag = 0;
	Udp_initServer(&terminate_flag);

	Udp_join();

	Udp_cleanup();
}

void testLed()
{
	terminate_flag = 0;
	Led_init(&terminate_flag);

	Led_joinThreads();
	Led_cleanUp();
}

int main()
{	
	testLed();
	return 0;
}
