#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>
#include "../include/app_sampler.h"
#include "../include/app_upd.h"
#include "../include/app_helper.h"
#include "../../hal/include/led_P921.h"

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
	led_init();
	
	while(1)
	{

	}
}

int main()
{	
	operation();
	return 0;
}
