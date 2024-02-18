#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <stdbool.h>
#include <pthread.h>
#include <semaphore.h>
#include "../../hal/include/a2d.h"
#include "../include/app_helper.h"

//Trigger
static int *isTerminated;

//Resources - calculation
static double arr_rawData[1000];
static double previous_avg;
static double previous_sum;
static int batch_size;

//Resources - for user
static double *arr_historyData;
static int count = 0;
static long dips = 0;
static long long length = 0;

//Thread
static pthread_t a2d_id;

//Mutex
pthread_mutex_t mutex;

//Semaphore
sem_t sem_full;
sem_t sem_empty;

//Initiate function
void *a2d_thread();

/*-------------------------- Public -----------------------------*/

//Getter to get previous count
long Sampler_getHistorySize(void)
{
    return count;
}

//Getter to get dips
long Sampler_getDips(void)
{
    return dips;
}

//Getter to get length
long long Sampler_getNumSamplesTaken(void) 
{
    return length;
}

//Getter to get previous avg
double Sampler_getAverageReading(void)
{
    return previous_avg;
}

// Clean up function
void Sampler_cleanup(void)
{   
    isTerminated = NULL;
    for(int i = 0; i < 1000; i++)
    {
        arr_rawData[i] = 0;
    }

    //free arr_historyData
    if(arr_historyData) {
        free(arr_historyData);
        arr_historyData = NULL;
    }

    //destroy mutex & semaphore
	pthread_mutex_destroy(&mutex);
    sem_destroy(&sem_full);
	sem_destroy(&sem_empty);
}

double *Sampler_getHistory(int *size)
{
    size = &count;
    return arr_historyData;
}

//Join
void Sampler_join(void)
{
    pthread_join(a2d_id, NULL);
}

//Init sampler thread
void Sampler_init(int *terminate_flag)
{
    //Trigger the start of the program
    isTerminated = terminate_flag;
    
    //Create & start a2d thread
    if(pthread_create(&a2d_id, NULL, a2d_thread, NULL) != 0) {
        exit(EXIT_FAILURE);
    }
}

/*-------------------------- Private -----------------------------*/

// thread to read input from A2D device
void *producer_thread() 
{
    long long currentTime;
    long long startTime;

    //while isTerminated == false => keep executing
    while(isTerminated == 0){
        batch_size = 0;
        currentTime = 0;
        startTime = getTimeInMs();

        //Wait for sem_empty -> 1 -> obtain -> decrement
        sem_wait(&sem_empty);
        pthread_mutex_lock(&mutex);

        //Keep reading data for 1000 ms
        while((currentTime = getTimeInMs() - startTime) < 1000) 
        {
            //Produce new data here
            sleepForMs(200);
            int reading = getVoltage0Read();
            double voltageToStore = getVoltageConvert(reading);

            //Store sample of current second
            arr_rawData[batch_size] = voltageToStore;

            //Update sum & sample_size - need to do before calculate average           
            previous_sum += voltageToStore;
            batch_size++;

            //length need continuously update
            length++;       

            //Update previous average - this is overall average - not tight to the batch
            if(length == 1){
                previous_avg = calculateSimpleAvg(length, previous_sum);
            }
            else{
                previous_avg = exponentSmoothAvg(calculateSimpleAvg(length, previous_sum), previous_avg);   
            }

            //Add here function keep track of dip
            printf("Time: %lld Sample size: %lld Value %5.3f ==> sum:%5.3f avg:%5.3fV\n", currentTime, length, voltageToStore, previous_sum, previous_avg);
        }

        //Unlock thread & increment sem_full -> ready to transfer
        pthread_mutex_unlock(&mutex);
        sem_post(&sem_full);
        
        //sleep for 1ms - before next iteration
        sleepForMs(1);
    }

    return NULL;
}


void *consumer_thread()
{
    while(isTerminated == 0)
    {
        //Wait sem_full > 0 -> obtain -> decrement & lock mutex to access resource
        sem_wait(&sem_full);
        pthread_mutex_lock(&mutex);

        //Update the batch size for the completed iteration
        count = batch_size;
        
        //free previous array - deallocate & allocate new memory
        if(arr_historyData)
        {
            free(arr_historyData);
            arr_historyData = NULL;
        }
        arr_historyData = (int *) malloc((count) * sizeof(int));
        
        //Transfer data
        for(int i = 0; i < count; i++)
        {
            arr_historyData[i] = arr_rawData[i];
        }

        //Unlock mutex -> increment sem_empty -> allow producer to generate 
        pthread_mutex_unlock(&mutex);
        sem_post(&sem_empty);
    }

    return NULL;
}