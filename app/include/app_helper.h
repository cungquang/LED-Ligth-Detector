#ifndef _APP_HELPER_H_
#define _APP_HELPER_H_

#include <stdbool.h>

// This module provide support functions (application level)
// - retrieve time in ms
// - sleep for an amount of time in ms
// - calculate exponential smoothing average
// - calculate regular/simple average
// - convert integer to string
// - merge an string to another string
// - validate if string is empty

long long getTimeInMs(void);
void sleepForMs(long long delayInMs);
bool isEmptyString(const char *strToCheck);
double exponentSmoothAvg(double current_avg, double previous_avg);
double calculateSimpleAvg(long current_size, double current_sum);
const char *convertDataToString(int *char_size, double data);
void mergeToBuffer(char *buffer, int *buffer_size, const char *number, int number_size);

#endif