#include <stdio.h>
#include <stdlib.h>
#include "../include/app_sampler.h"
#include "../include/app_upd.h"
#include "../include/app_ledP921.h"
#include "../include/app_i2c.h"

int terminate_flag = 0;
int cleanUp_flag = 0;

void operation()
{
	I2C_init(&terminate_flag);
	LED_init(&terminate_flag);
	UDP_initServer(&terminate_flag);
	SAMPLER_init(&terminate_flag);
	SHUTDOWN_init(&cleanUp_flag);

	//Terminate all main threads
	I2C_join();
	LED_join();
	UDP_join();
	SAMPLER_join();

	//Trigger shutdown thread -> clean up
	cleanUp_flag = 1;
	
}

int main() 
{
	operation();
    return 0;
}
