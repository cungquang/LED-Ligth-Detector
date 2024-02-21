#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <stdbool.h>
#include <pthread.h>
#include <semaphore.h>
#include "../../hal/include/led_P921.h"
#include "../../hal/include/a2d.h"
#include "../include/app_helper.h"

static int *isTerminated;
static int gapTimeInBetween;

//Thread
static pthread_t led_on_id;
static pthread_t led_off_id;

//Mutex
pthread_mutex_t led_mutex;

//Semaphore
sem_t led_on;
sem_t led_off;

void *Led_turnOn();
void *Led_turnOff();

void Led_init(int *terminate_flag)
{   
    isTerminated = terminate_flag;
    gapTimeInBetween = 200;
    led_configure();
    led_writeToPeriod(led_getPeriod());
    led_writeToDutyCycle(led_getDutyCycle());
    led_writeToEnable(1);

    //init mutex & semaphore
	pthread_mutex_init(&led_mutex, NULL);
	sem_init(&led_on, 0, 1);
    sem_init(&led_off, 0, 0); 

    //Create & start producer_thread
    if(pthread_create(&led_on_id, NULL, Led_turnOn, NULL) != 0) {
        exit(EXIT_FAILURE);
    }

    //Create & start consumer_thread
    if(pthread_create(&led_off_id, NULL, Led_turnOff, NULL) != 0) {
        exit(EXIT_FAILURE);
    }
}

void Led_joinThreads()
{
    pthread_join(led_off_id, NULL);
    pthread_join(led_on_id, NULL);
}

void Led_cleanUp()
{
    isTerminated = NULL;

    //Setup for led
    led_writeToEnable(0);
    led_writeToDutyCycle(0);

    //Create mutex & semaphore
    pthread_mutex_destroy(&led_mutex);
    sem_destroy(&led_on);
	sem_destroy(&led_off);
}


//////////////////////////////////////////// PRIVATE ////////////////////////////////////////////

void *Led_turnOn()
{  
    while(!*isTerminated)
    {
        gapTimeInBetween = A2D_readFromVoltage0()/40;
        
        //Wait for led_on > 0 => decrement
        sem_wait(&led_on);
        pthread_mutex_lock(&led_mutex);

        led_writeToDutyCycle(led_getDutyCycle());
        sleepForMs(gapTimeInBetween);

        pthread_mutex_unlock(&led_mutex);
        sem_post(&led_off);
    }

    return NULL;
}

void *Led_turnOff()
{
    while(!*isTerminated)
    {
        gapTimeInBetween = A2D_readFromVoltage0()/40;

        //Wait until led_off > 0 => decrement 
        sem_wait(&led_off);
        pthread_mutex_lock(&led_mutex);

        led_writeToDutyCycle(0);
        sleepForMs(gapTimeInBetween);

        pthread_mutex_unlock(&led_mutex);
        sem_post(&led_on);
    }

    return NULL;
}