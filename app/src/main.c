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
#include "../include/periodTimer.h"


int terminate_flag = 0;

void testSampler()
{
	I2C_init(&terminate_flag);
	LED_init(&terminate_flag);
	UDP_initServer(&terminate_flag);
	SAMPLER_init(&terminate_flag);

	I2C_join();
	LED_join();
	UDP_join();
	SAMPLER_join();

	UDP_cleanup();
	SAMPLER_cleanup();
}

int main() 
{
	testSampler();
    return 0;
}
