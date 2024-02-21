#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>
#include "../include/app_sampler.h"
#include "../include/app_upd.h"
#include "../include/app_helper.h"
#include "../include/app_ledP921.h"
#include "../include/app_i2c.h"


int terminate_flag = 0;

void operation()
{
	Udp_initServer(&terminate_flag);

	Udp_join();

	Udp_cleanup();
}

void testLed()
{
	Led_init(&terminate_flag);

	Led_joinThreads();
	Led_cleanUp();
}

void testI2C()
{
	I2C_init(&terminate_flag);

	I2C_join();
}

void testSampler()
{
	
}

int main()
{	
	return 0;
}
