#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "../include/app_sampler.h"
#include "../include/app_upd.h"
#include "../include/app_ledP921.h"
#include "../include/app_i2c.h"

static int *isTimeToCleanUp;

static pthread_t shutdown_id;

void SHUTDOWN_init(int cleanUpFlag)
{
    isTimeToCleanUp = cleanUpFlag;

    //Create & start producer_thread
    if(pthread_create(&shutdown_id, NULL, SHUTDOWN_cleanUpThread, NULL) != 0) {
        exit(EXIT_FAILURE);
    }
}

void SHUTDOWN_join()
{
    pthread_join(shutdown_id, NULL);
}

////////////////////////////////////////////// PRIVATE //////////////////////////////////////////////

void *SHUTDOWN_cleanUpThread()
{
	I2C_cleanUp();
	LED_cleanUp();
	UDP_cleanup();
	SAMPLER_cleanup();
}