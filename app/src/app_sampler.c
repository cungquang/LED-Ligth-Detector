#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <stdbool.h>
#include <pthread.h>
#include <semaphore.h>
#include "../../hal/include/a2d.h"
#include "../include/app_helper.h"

//Trigger
static bool isTerminated;
static bool isDoneProduced;

//Resources
static long count = 0;
static long long length = 0;

static double arr_rawData[1000];
static double previous_avg;
static double previous_sum;

static double *arr_history;
static long count_history;

//Thread
static pthread_t a2d_id;
void *a2d_thread();

/*-------------------------- Public -----------------------------*/

//Init sampler thread
void Sampler_init(bool terminate_flag)
{
    //Trigger the start of the program
    setTerminate(terminate_flag);
    
    //Create & start a2d thread
    if(pthread_create(&a2d_id, NULL, a2d_thread, NULL) != 0) {
        exit(EXIT_FAILURE);
    }
}

// Clean up function
void Sampler_cleanup()
{
    //shutdown everything
    pthread_join(a2d_id, NULL);
    void closeFile();

    return NULL;
}

//Setter to set terminate_flag - end program
void Sampler_setTerminate(bool terminate_flag) {
    isTerminated = terminate_flag;
}

//Getter to get previous count
int Sampler_getHistorySize(void)
{
    return count;
}

//Getter to get previous avg
double Sampler_getAverageReading(void)
{
    return previous_avg;
}

//Setter to get length
long long Sampler_getNumSamplesTaken(void) 
{
    return length;
}

/*-------------------------- Private -----------------------------*/

// thread to read input from A2D device
void *a2d_thread() 
{
    long long batch_size;
    long long currentTime;
    long long startTime;
    //while isTerminated == false => keep executing
    while(!isTerminated){
        batch_size = 0;
        currentTime = 0;
        startTime = getTimeInMs();

        //Keep reading data for 1000 ms
        while((currentTime = getTimeInMs() - startTime) < 1000) 
        {
            int reading = getVoltage0Read();
            double voltageToStore = getVoltageConvert(reading);

            //Store sample of current second
            arr_rawData[batch_size] = voltageToStore;

            //Update sum & sample_size - need to do before calculate average           
            previous_sum += voltageToStore;
            batch_size++;

            //Update previous average - this is overall average - not tight to the batch
            if(length == 1){
                previous_avg = calculateSimpleAvg(length, previous_sum);
            }
            else{
                previous_avg = exponentSmoothAvg(calculateSimpleAvg(length, previous_sum), previous_avg);   
            }

            //Add here function keep track of dip
            printf("Time: %lld Sample size: %lld Value %5d ==> %5.3fV\n", currentTime, batch_size, voltageToStore, previous_avg);
        }
        //create a copy - get history

        //Update count for this batch
        count = batch_size;

        //Update length after this batch:
        length += batch_size;

        //sleep for 1ms - before next iteration
        sleepForMs(1);
    }

    return NULL;
}


// double *Sampler_getHistory(int *size)
// {

// }