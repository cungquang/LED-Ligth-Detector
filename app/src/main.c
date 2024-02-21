#include <stdio.h>
#include <stdlib.h>
#include "../include/app_sampler.h"
#include "../include/app_upd.h"
#include "../include/app_ledP921.h"
#include "../include/app_i2c.h"

int terminate_flag = 0;

void operation()
{
	I2C_init(&terminate_flag);
	LED_init(&terminate_flag);
	UDP_initServer(&terminate_flag);
	SAMPLER_init(&terminate_flag);

	I2C_join();
	LED_join();
	UDP_join();
	SAMPLER_join();

	I2C_cleanUp();
	LED_cleanUp();
	UDP_cleanup();
	SAMPLER_cleanup();
}

int main() 
{
	Operation();
    return 0;
}
