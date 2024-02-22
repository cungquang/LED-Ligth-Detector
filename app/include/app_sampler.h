#ifndef _APP_SAMPLER_H_
#define _APP_SAMPLER_H_

// This module manages the operation of Sampling process (application level)
// - Initiate all threads for generating sample, storing sample and analyze statistical data
// - Join threads after complete
// - terminate & clean up the thread
// - access to key data: dips, batch size, total sampling size, historical data

//Thread
void SAMPLER_init(int *terminate_flag);

//Cleanup
void SAMPLER_cleanup(void);

//Join
void SAMPLER_join(void);

// Getter
long long SAMPLER_getCount(void);
int SAMPLER_getDips(void);
double SAMPLER_getAverageReading(void);
int SAMPLER_getLength(void); 
double *SAMPLER_getHistory(int *size);

#endif