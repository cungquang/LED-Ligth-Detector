#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <stdbool.h>
#include <pthread.h>
#include "../../hal/include/led_P921.h"
#include "../../hal/include/a2d.h"
#include "../include/app_helper.h"

static int *isTerminated;
static int potRaw;
static int potHz;

//Thread
static pthread_t led_flash_id;
static pthread_t pot_read_id;

//Initiate function
void *LED_flashLedThread();
void *LED_getPotThread();

//////////////////////////////////////////// PUBLIC ////////////////////////////////////////////

void LED_init(int *terminate_flag)
{   
    isTerminated = terminate_flag;
    potRaw = 200;
    led_configure();
    led_writeToPeriod(led_getPeriod());
    led_writeToDutyCycle(led_getDutyCycle());
    led_writeToEnable(1);

    //Create & start LED_getPotThread
    if(pthread_create(&pot_read_id, NULL, LED_getPotThread, NULL) != 0) {
        exit(EXIT_FAILURE);
    }

    //Create & start LED_flashLedThread
    if(pthread_create(&led_flash_id, NULL, LED_flashLedThread, NULL) != 0) {
        exit(EXIT_FAILURE);
    }
}

void LED_getPot()
{
    return potRaw;
}

void LED_getPotHz()
{
    return potHz;
}

void LED_join()
{
    pthread_join(led_flash_id, NULL);
}

void LED_cleanUp()
{
    isTerminated = NULL;

    //Setup for led
    led_writeToEnable(0);
    led_writeToDutyCycle(0);
}


//////////////////////////////////////////// PRIVATE ////////////////////////////////////////////

void *LED_flashLedThread()
{
    while(!*isTerminated)
    {        
        //turn on the led
        led_writeToDutyCycle(led_getDutyCycle());
        sleepForMs(potRaw);

        //turn off the led
        led_writeToDutyCycle(0);
        sleepForMs(potRaw);
    }

    return NULL;
}

void *LED_getPotThread()
{
    while(!*isTerminated)
    {
        potRaw = A2D_readFromVoltage0();
        potHz = potRaw/40;

        //Sleep for 100 second before the next iteration
        sleepForMs(100);
    }
}
