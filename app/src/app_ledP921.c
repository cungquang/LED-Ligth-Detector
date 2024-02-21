#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <stdbool.h>
#include <pthread.h>
#include "../../hal/include/led_P921.h"
#include "../../hal/include/a2d.h"
#include "../include/app_helper.h"

static int *isTerminated;
static int gapTimeInBetween;

//Thread
static pthread_t led_flash_id;

//Initiate function
void *LED_flashLed();

//////////////////////////////////////////// PUBLIC ////////////////////////////////////////////

void LED_init(int *terminate_flag)
{   
    isTerminated = terminate_flag;
    gapTimeInBetween = 200;
    led_configure();
    led_writeToPeriod(led_getPeriod());
    led_writeToDutyCycle(led_getDutyCycle());
    led_writeToEnable(1);

    //Create & start producer_thread
    if(pthread_create(&led_flash_id, NULL, LED_flashLed, NULL) != 0) {
        exit(EXIT_FAILURE);
    }
}

void LED_join()
{
    pthread_join(led_flash_id, NULL);
}

// void LED_cleanUp()
// {
//     isTerminated = NULL;

//     //Setup for led
//     led_writeToEnable(0);
//     led_writeToDutyCycle(0);
// }


//////////////////////////////////////////// PRIVATE ////////////////////////////////////////////

void *LED_flashLed()
{
    while(!*isTerminated)
    {
        gapTimeInBetween = A2D_readFromVoltage0()/40;
        
        //turn on the led
        led_writeToDutyCycle(led_getDutyCycle());
        sleepForMs(gapTimeInBetween);

        //turn off the led
        led_writeToDutyCycle(0);
        sleepForMs(gapTimeInBetween);
    }

    return NULL;
}
