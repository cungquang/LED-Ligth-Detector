#include <time.h>
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>		// Errors
#include <string.h>
#include <sys/epoll.h>  // for epoll()
#include <fcntl.h>      // for open()
#include <unistd.h>     // for close()


static double weighted = 0.999;

long long getTimeInMs(void)
{
    struct timespec spec;
    clock_gettime(CLOCK_REALTIME, &spec);
    long long seconds = spec.tv_sec;
    long long nanoSeconds = spec.tv_nsec;
    long long milliSeconds = seconds * 1000
    + nanoSeconds / 1000000;
    return milliSeconds;
}

void sleepForMs(long long delayInMs)
{
    const long long NS_PER_MS = 1000 * 1000;
    const long long NS_PER_SECOND = 1000000000;
    long long delayNs = delayInMs * NS_PER_MS;
    int seconds = delayNs / NS_PER_SECOND;
    int nanoseconds = delayNs % NS_PER_SECOND;
    struct timespec reqDelay = {seconds, nanoseconds};
    nanosleep(&reqDelay, (struct timespec *) NULL);
}

bool isEmptyString(const char *strToCheck)
{
    return strlen(strToCheck) == 0;
}

double exponentSmoothAvg(double current_avg, double previous_avg)
{
    return current_avg*weighted + previous_avg*(1-weighted);
}

double calculateSimpleAvg(long current_size, double current_sum) 
{
    return current_sum/current_size;
}

char *convertDataToString(int *char_size, double data) 
{
    //pre-calculate the length need for data
    *char_size = snprintf(NULL, 0, "%5.3f", data);
    char *number = (char *)malloc((*char_size + 1)*sizeof(char));      //add null pointer at the end

    if(number == NULL) 
    {
        perror("Fail to allocate memory");
        exit(EXIT_FAILURE);
    }

    //transfer data into number (str)
    snprintf(number, *char_size + 1, "%5.3f", data);
    return number;
}

//Source: ChatGPT
void mergeToBuffer(char *buffer, int *buffer_size, const char *number, int number_size)
{
    //Copy char in number into buffer (with number_size) - buffer + *buffer_size - memory address where to start writting
    //first to buffer
    if(*buffer_size == 0)
    {
        memcpy(buffer + *buffer_size, number, number_size);
    }
    //all number after the first 
    else{
        //add ',' between number
        buffer[(*buffer_size)++] = ',';
        memcpy(buffer + *buffer_size, number, number_size);
    }

    //update buffer size
    *buffer_size += number_size;

    //Add null-terminate
    buffer[*buffer_size] = '\0';
}

double convertToPwmFrequency(double rawData)
{
    return rawData/40;
}