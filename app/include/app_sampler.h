#ifndef _APP_SAMPLER_H_
#define _APP_SAMPLER_H_

//Thread
void SAMPLER_init(int *terminate_flag);

//Cleanup
void SAMPLER_cleanup(void);

//Join
void SAMPLER_join(void);

// Getter
int SAMPLER_getHistorySize(void);
int SAMPLER_getDips(void);
double SAMPLER_getAverageReading(void);
long long SAMPLER_getNumSamplesTaken(void); 
double *SAMPLER_getHistory(int *size);

//test
double * SAMPLER_testHistory(int *size);

#endif