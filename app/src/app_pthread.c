#include <stdio.h>
#include <signal.h>
#include <stdbool.h>
#include <pthread.h>
#include "../../hal/include/a2d.h"
#include "../include/app_helper.h"

static long long count = 0;
static long long prev_length = 0;
static bool isTerminated;
static pthread_t a2d_id;
static pthread_t shutdown_id;
static pthread_t udpIn_id;

void *shutdown_thread();
void *a2d_thread();
void *udpIn_thread();

void setTerminate(bool terminate_flag) {
    isTerminated = terminate_flag;
}

int init_thread(bool terminate_flag)
{
    //Trigger the start of the program
    setTerminate(terminate_flag);

    // Create & start shutdown thread
    if(pthread_create(&shutdown_id, NULL, shutdown_thread, NULL) != 0){
        return 1;
    }

    //Create & start a2d thread
    if(pthread_create(&a2d_id, NULL, a2d_thread, NULL) != 0) {
        return 1;
    }

    return 0;
}

void *a2d_thread() 
{
    //while isTerminated == false => keep executing
    while(!isTerminated){
        long long sample_size = 0;
        long long currentTime;
        long long startTime = getTimeInMs();

        //Keep reading data for 1000 ms
        while((currentTime = getTimeInMs() - startTime) < 1000) 
        {
            int reading = getVoltage0Read();
            double voltage = getVoltageConvert(reading);
            sample_size++;

            //Store sample of current second

            //Add here function keep track of dip
            printf("Time: %lld Sample size: %lld Value %5d ==> %5.3fV\n", currentTime, sample_size, reading, voltage);
        }

        // Update prev_length & count
        prev_length = sample_size;
        count += sample_size;

        //sleep for 1ms
        sleepForMs(1);
    }

    return NULL;
}

void *shutdown_thread()
{
    //sleep until isTerminated == true => start clean upcl
    while(!isTerminated)
    {
        sleepForMs(1);
    }

    //shutdown everything
    pthread_join(a2d_id, NULL);
    pthread_join(shutdown_id, NULL);
    void closeFile();

    return NULL;
}
