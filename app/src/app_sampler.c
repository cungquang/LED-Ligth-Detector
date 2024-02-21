#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <stdbool.h>
#include <pthread.h>
#include <semaphore.h>
#include "../../hal/include/a2d.h"
#include "../include/app_helper.h"

#define MAX_BUFFER_SIZE 1000

//Trigger
static int *isTerminated;

//Resources - current
static double rawData;
static double arr_rawData[MAX_BUFFER_SIZE];
static double previous_avg;
static double previous_sum;
static int batch_size;

//Resources - history
static int count = 0;
static int dips = 0;
static long long length = 0;

//Resources - send
static double *arr_historyToSend;

//Thread
static pthread_t producer_id;
static pthread_t consumer_id;

//Mutex
pthread_mutex_t sampler_mutex;

//Semaphore
sem_t sampler_full;
sem_t sampler_empty;

//Initiate function
void *SAMPLER_producerThread();
void *SAMPLER_consumerThread();
void SAMPLER_calculateAverage();

/////////////////////////////////////////// PUBLIC ///////////////////////////////////////////

double * SAMPLER_testHistory(int *size)
{
    // Dynamically allocate memory for the array
    double *strNum = malloc(8 * sizeof(double));

    // Initialize the array
    strNum[0] = 123.3425;
    strNum[1] = 3431.1234;
    strNum[2] = 23123.3416;
    strNum[3] = 7778.96;
    strNum[4] = 233.397;
    strNum[5] = 897.3416;
    strNum[6] = 7873.123;
    strNum[7] = 721.345;

    // Update the size
    *size = 8;

    return strNum;
}

//Getter to get previous count
int SAMPLER_getHistorySize(void)
{
    return count;
}

//Getter to get dips
int SAMPLER_getDips(void)
{
    return dips;
}

//Getter to get length
long long SAMPLER_getNumSamplesTaken(void) 
{
    return length;
}

//Getter to get previous avg
double SAMPLER_getAverageReading(void)
{
    return previous_avg;
}

//Getter to get history data
double *SAMPLER_getHistory(int *size)
{
    pthread_mutex_lock(&sampler_mutex);
    *size = count;
    arr_historyToSend = (double *)malloc((*size) * sizeof(double));

    for(int i = 0; i < *size; i++)
    {
        arr_historyToSend[i] = arr_rawData[i];
    }

    pthread_mutex_unlock(&sampler_mutex);
    return arr_historyToSend;
}

// Clean up function
void SAMPLER_cleanup(void)
{   
    isTerminated = NULL;
    for(int i = 0; i < 1000; i++)
    {
        arr_rawData[i] = 0;
    }

    //free arr_historyToSend
    if(arr_historyToSend) 
    {
        free(arr_historyToSend);
        arr_historyToSend = NULL;
    }

    //destroy mutex & semaphore
	pthread_mutex_destroy(&sampler_mutex);
    sem_destroy(&sampler_full);
	sem_destroy(&sampler_empty);
}

//Join
void SAMPLER_join(void)
{
    pthread_join(producer_id, NULL);
    pthread_join(consumer_id, NULL);
}

//Init sampler thread
void SAMPLER_init(int *terminate_flag)
{
    //Trigger the start of the program
    isTerminated = terminate_flag;
    
    //init mutex & semaphore
	pthread_mutex_init(&sampler_mutex, NULL);

	//1st para: address to semaphore
	//2nd para: number of process sharing this semaphore
	//3rd para: initial value
	sem_init(&sampler_empty, 0, 1);
    sem_init(&sampler_full, 0, 0); 


    //Create & start producer_thread
    if(pthread_create(&producer_id, NULL, SAMPLER_producerThread, NULL) != 0) {
        exit(EXIT_FAILURE);
    }

    //Create & start consumer_thread
    if(pthread_create(&consumer_id, NULL, SAMPLER_consumerThread, NULL) != 0) {
        exit(EXIT_FAILURE);
    }
}

/////////////////////////////////////////// Private ///////////////////////////////////////////

// thread to read input from A2D device
void *SAMPLER_producerThread() 
{
    //while isTerminated == false => keep executing
    while(!*isTerminated){

        //Wait for sem_empty -> 1 -> obtain -> decrement
        sem_wait(&sampler_empty);
        pthread_mutex_lock(&sampler_mutex);

        //Produce new data here
        rawData = A2D_convertVoltage(A2D_readFromVoltage1());

        //Unlock thread & increment sem_full -> ready to transfer
        pthread_mutex_unlock(&sampler_mutex);
        sem_post(&sampler_full);
    }

    return NULL;
}


void *SAMPLER_consumerThread()
{
    long long currentTime;
    long long startTime;

    while(!*isTerminated)
    {
        batch_size = 0;
        currentTime = 0;
        startTime = getTimeInMs();

        //Consume data within 1000 ms
        while((currentTime = getTimeInMs() - startTime) < 1000) 
        {
            //Wait sem_full > 0 -> obtain -> decrement & lock mutex to access resource
            sem_wait(&sampler_full);
            pthread_mutex_lock(&sampler_mutex);
            
            //Transfer data
            arr_rawData[batch_size++] = rawData;
            length++;       

            //Unlock mutex -> increment sem_empty -> allow producer to generate more products
            pthread_mutex_unlock(&sampler_mutex);
            sem_post(&sampler_empty);

            //Update sum & sample_size - need to do before calculate average           
            previous_sum += arr_rawData[batch_size - 1];

            //length need continuously update
            SAMPLER_calculateAverage();
        
            //Add here function keep track of dip
            printf("Time: %lld Sample size: %lld Value %5.3f ==> sum:%5.3f avg:%5.3fV\n", currentTime, length, arr_rawData[batch_size], previous_sum, previous_avg);
        }
        
        //Update count
        count = batch_size;

        //sleep for 1ms - before next iteration
        sleepForMs(1);
    }

    return NULL;
}

void SAMPLER_calculateAverage()
{
    //Update previous average - this is overall average - not tight to the batch
    if(length == 1){
        previous_avg = calculateSimpleAvg(length, previous_sum);
    }
    else{
        previous_avg = exponentSmoothAvg(calculateSimpleAvg(length, previous_sum), previous_avg);   
    }
}