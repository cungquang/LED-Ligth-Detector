#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <stdbool.h>
#include <pthread.h>
#include <semaphore.h>
#include "../../hal/include/a2d.h"
#include "../include/app_i2c.h"
#include "../include/periodTimer.h"
#include "../include/app_helper.h"

#define MAX_BUFFER_SIZE 3000

//Trigger
static int *isTerminated;

//Resources - current
static Period_statistics_t stats;
static double rawData;
static double arr_rawData[MAX_BUFFER_SIZE];
static double arr_historyData[MAX_BUFFER_SIZE];
static double accumulate_sum = 0;
static double previous_voltage = 0;
static double previous_avg = 0;
static double current_voltage;
static double current_avg = 0;
static int batch_dips = 0;
static int batch_size;

//Resources - for accessing
static int count = 0;
static int dips = 0;
static long long length = 0;
static double min_period = 0;
static double max_period = 0;
static double avg_period = 0;

//Resources - send
static double *arr_historyToSend;

//Thread
static pthread_t producer_id;
static pthread_t consumer_id;
static pthread_t analyzer_id;

//Mutex
pthread_mutex_t sampler_mutex;
pthread_mutex_t stats_mutex;

//Semaphore
sem_t sampler_full;
sem_t sampler_empty;
sem_t stats_old;
sem_t stats_new;

//Initiate function
void *SAMPLER_producerThread();
void *SAMPLER_consumerThread();
void *SAMPLER_analyzerThread();
void SAMPLER_calculateAverage();
void SAMPLER_calculateDip();

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
    pthread_mutex_lock(&stats_mutex);
    *size = count;
    arr_historyToSend = (double *)malloc((*size) * sizeof(double));

    for(int i = 0; i < *size; i++)
    {
        arr_historyToSend[i] = arr_historyData[i];
    }
    pthread_mutex_unlock(&stats_mutex);

    return arr_historyToSend;
}

// Clean up function
void SAMPLER_cleanup(void)
{   
    isTerminated = NULL;
    
    //Clear data
    for(int i = 0; i < MAX_BUFFER_SIZE; i++)
    {
        arr_rawData[i] = 0;
        arr_historyData[i] = 0;
    }

    //Free arr_historyToSend
    if(arr_historyToSend) 
    {
        free(arr_historyToSend);
        arr_historyToSend = NULL;
    }

    //Delete or clean period
    Period_cleanup();

    //destroy mutex & semaphore
	pthread_mutex_destroy(&sampler_mutex);
    pthread_mutex_destroy(&stats_mutex);

    sem_destroy(&sampler_full);
	sem_destroy(&sampler_empty);
    sem_destroy(&stats_new);
    sem_destroy(&stats_old);
}

//Join
void SAMPLER_join(void)
{
    pthread_join(producer_id, NULL);
    pthread_join(consumer_id, NULL);
    pthread_join(analyzer_id, NULL);
}

//Init sampler thread
void SAMPLER_init(int *terminate_flag)
{
    //Trigger the start of the program
    isTerminated = terminate_flag;
    
    //init mutex
	pthread_mutex_init(&sampler_mutex, NULL);
    pthread_mutex_init(&stats_mutex, NULL);

	//init semaphore
	sem_init(&sampler_empty, 0, 1);
    sem_init(&sampler_full, 0, 0); 

    sem_init(&stats_old, 0, 1);
    sem_init(&stats_new, 0, 0);

    //Initiate Period Timer
    Period_init();

    //Create & start producer_thread
    if(pthread_create(&producer_id, NULL, SAMPLER_producerThread, NULL) != 0) {
        exit(EXIT_FAILURE);
    }

    //Create & start consumer_thread
    if(pthread_create(&consumer_id, NULL, SAMPLER_consumerThread, NULL) != 0) {
        exit(EXIT_FAILURE);
    }

    //Create & start analyzer_thread
    if(pthread_create(&analyzer_id, NULL, SAMPLER_analyzerThread, NULL) != 0) {
        exit(EXIT_FAILURE);
    }
}

/////////////////////////////////////////// Private ///////////////////////////////////////////

//Produce (read) data from A2D
void *SAMPLER_producerThread() 
{
    //while isTerminated == false => keep executing
    while(!*isTerminated){

        //Wait for sem_empty -> 1 -> obtain -> decrement
        sem_wait(&sampler_empty);
        pthread_mutex_lock(&sampler_mutex);

        //Mark statistic event
        Period_markEvent(PERIOD_EVENT_SAMPLE_LIGHT);
        
        //Produce new data here
        rawData = A2D_convertVoltage(A2D_readFromVoltage1());

        sleepForMs(40);

        //Unlock thread & increment sem_full -> ready to transfer
        pthread_mutex_unlock(&sampler_mutex);
        sem_post(&sampler_full);
    }

    return NULL;
}

//Consume data from Producer
void *SAMPLER_consumerThread()
{
    long long currentTime;
    long long startTime;

    while(!*isTerminated)
    {
        //Reset
        batch_size = 0;
        batch_dips = 0;
        previous_voltage = 0;
        current_voltage = 0;
        currentTime = 0;
        startTime = getTimeInMs();

        //Consume data within 1000 ms OR 1 second
        while((currentTime = getTimeInMs() - startTime) < 1000) 
        {
            //Update previous value
            previous_voltage = current_voltage;
            previous_avg = current_avg;

            //Wait sem_full > 0 -> obtain -> decrement & lock mutex to access resource
            sem_wait(&sampler_full);
            pthread_mutex_lock(&sampler_mutex);

            //Transfer data
            arr_rawData[batch_size] = rawData;
            current_voltage = rawData;
            accumulate_sum += rawData;
            
            //Update length & batch_size
            batch_size++;
            length++;       

            //length need continuously update
            SAMPLER_calculateAverage();
            SAMPLER_calculateDip();

            printf("dips-%d\t\tCurr_Raw-%.3f\t\tCurr_Avg-%.3f\t\tPrev_Raw-%.3f\t\tPrev_Avg-%.3f\n", batch_dips, current_voltage, current_avg, previous_voltage, previous_avg);

            //Unlock mutex -> increment sem_empty -> allow producer to generate more products
            pthread_mutex_unlock(&sampler_mutex);
            sem_post(&sampler_empty);
        }

        //Wait for old stats -> to calculate
        sem_wait(&stats_old);
        pthread_mutex_lock(&stats_mutex);

        count = batch_size;
        dips = batch_dips;
        
        //Create a new stats -> to trigger update
        pthread_mutex_unlock(&stats_mutex);
        sem_post(&stats_new);

        //sleep for 1ms - before next iteration
        sleepForMs(1);
    }

    return NULL;
}

void *SAMPLER_analyzerThread()
{
    while(!*isTerminated)
    {
        //Wait for new stats - to be produce
        sem_wait(&stats_new);
        pthread_mutex_lock(&stats_mutex);

        //Move data to arr_historydata
        for(int i = 0; i < count; i++)
        {
            arr_historyData[i] = arr_rawData[i];
        }

        I2C_setDipsToDisplay(dips);

        //Get statistic
        Period_getStatisticsAndClear(PERIOD_EVENT_SAMPLE_LIGHT, &stats);
        min_period = stats.minPeriodInMs;
        max_period = stats.maxPeriodInMs;
        avg_period = stats.avgPeriodInMs;

        //Create one new old stats
        pthread_mutex_unlock(&stats_mutex);
        sem_post(&stats_old);

        //Print line 1
        printf("Smpl/s = %d\tPOT @\tavg = %.3fV\tdips = %d\tSmpl ms[%.3f, %.3f] avg %.3f/%d\n",count, current_avg, dips, min_period, max_period, avg_period, count);

        //Print line 2
    }

    return NULL;
}

//Average light level is independent from the batch
void SAMPLER_calculateAverage()
{
    //Update previous average - this is overall average - not tight to the batch
    if(length == 1){
        current_avg = calculateSimpleAvg(length, accumulate_sum);
    }
    else{
        current_avg = exponentSmoothAvg(calculateSimpleAvg(length, accumulate_sum), previous_avg);   
    }
}

void SAMPLER_calculateDip()
{
    //Update the dips - when at least 2 data points && previous already rise && current reduce by 0.1
    if(batch_size > 1 && (previous_avg - previous_voltage) <= 0.03 && (current_avg - current_voltage) >= 0.1)
    {
        batch_dips += 1;
    }
}