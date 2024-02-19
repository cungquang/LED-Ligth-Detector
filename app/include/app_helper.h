// Sample application module
// Normally, you might create a module such as this for each module in
// the application that does some high-level task; perhaps has its own thread.

#ifndef _APP_HELPER_H_
#define _APP_HELPER_H_

#include <stdbool.h>

long long getTimeInMs(void);
void sleepForMs(long long delayInMs);
bool isEmptyString(const char *strToCheck);
double exponentSmoothAvg(double current_avg, double previous_avg);
double calculateSimpleAvg(long current_size, double current_sum);
const char *convertDataToString(int *char_size, double data);
void mergeToBuffer(char *buffer, int *buffer_size, const char *number, int number_size);

#endif