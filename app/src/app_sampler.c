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
static double rawPot = 0;
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
static double pot = 0;
static int potToHz = 0;
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

//Initiate function
void *SAMPLER_producerThread();
void *SAMPLER_consumerThread();
void *SAMPLER_analyzerThread();
void SAMPLER_calculateAverage();
void SAMPLER_calculateDip();
void SAMPLER_print2ndLine();

/////////////////////////////////////////// PUBLIC ///////////////////////////////////////////

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
    //Free arr_historyToSend
    if(arr_historyToSend) 
    {
        arr_historyToSend = NULL;
    }

    //Delete or clean period
    Period_cleanup();

    //destroy mutex & semaphore
	pthread_mutex_destroy(&sampler_mutex);
    pthread_mutex_destroy(&stats_mutex);

    sem_destroy(&sampler_full);
	sem_destroy(&sampler_empty);
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

        //sleep for 1ms - before next iteration
        sleepForMs(1);

        //Unlock thread & increment sem_full -> ready to transfer
        pthread_mutex_unlock(&sampler_mutex);
        sem_post(&sampler_full);
    }

    return NULL;
}

//Consume data from Producer
void *SAMPLER_consumerThread()
{
    // long long currentTime;
    // long long startTime;

    while(!*isTerminated)
    {
        // currentTime = 0;
        // startTime = getTimeInMs();
        //Consume data within 1000 ms OR 1 second
        // while((currentTime = getTimeInMs() - startTime) < 1000) 

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

        //Unlock mutex -> increment sem_empty -> allow producer to generate more products
        pthread_mutex_unlock(&sampler_mutex);
        sem_post(&sampler_empty);

        //printf("dips-%d\t\tCurr_Raw-%.3f\t\tCurr_Avg-%.3f\t\tPrev_Raw-%.3f\t\tPrev_Avg-%.3f\n", batch_dips, current_voltage, current_avg, previous_voltage, previous_avg);
    }

    return NULL;
}

void *SAMPLER_analyzerThread()
{
    while(!*isTerminated)
    {
        sleepForMs(1000);

        //get dips and count
        dips = batch_dips;
        count = batch_size;

        //Reset
        batch_size = 0;
        batch_dips = 0;
        previous_voltage = 0;
        current_voltage = 0;

        //Reset & get statistic
        Period_getStatisticsAndClear(PERIOD_EVENT_SAMPLE_LIGHT, &stats);
        
        //Block access to critical section
        pthread_mutex_lock(&stats_mutex);

        //Copy data to arr_historydata
        for(int i = 0; i < count; i++)
        {
            arr_historyData[i] = arr_rawData[i];
        }

        count = stats.numSamples;
        min_period = stats.minPeriodInMs;
        max_period = stats.maxPeriodInMs;
        avg_period = stats.avgPeriodInMs;

        //Update key data
        potToHz = (pot + rawPot)/40;

        //Create one new old stats
        pthread_mutex_unlock(&stats_mutex);

        I2C_setDipsToDisplay(dips);

        //Print message to screen
        //printf("Smpl/s = %d\tPOT @ %.3f => %dHz\tavg = %.3fV\tdips = %d\tSmpl ms[%.3f, %.3f] avg %.3f/%d\n", count, pot, potToHz, current_avg, dips, min_period, max_period, avg_period, count);
        printf("Smpl/s = %d\tavg = %.3fV\tdips = %d\tSmpl ms[%.3f, %.3f] avg %.3f/%d\n", count, current_avg, dips, min_period, max_period, avg_period, count);
        SAMPLER_print2ndLine();
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
    //printf("prev_vol:%.3f\t\tprev_avg:%.3f\t\tcurr_vol:%.3f\t\tcurr_avg:%.3f\n", previous_voltage, previous_avg,current_voltage, current_avg);
    //printf("previous:%.3f - %.3f = %.3f \t\t\t current: %.3f - %.3f = %.3f\n", previous_avg, previous_voltage, previous_avg - previous_voltage, current_avg, current_voltage, current_avg - current_voltage);
    if((batch_size > 1) && (previous_avg - previous_voltage <= 0.07) && (current_avg - current_voltage >= 0.1))
    {   
        batch_dips += 1;
    }
}

void SAMPLER_print2ndLine()
{
    int batch_count = count;
    int incre = batch_count/20;
    if (incre < 1){
        incre = 1;
    }

    //Get 20 items
    for(int i = 0; i < batch_count; i += incre)
    {
        printf("\t%d:%.3f\t", i, arr_historyData[i]);
    }

    //Print new line
    printf("\n");
}