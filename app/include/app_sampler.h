#ifndef _APP_SAMPLER_H_
#define _APP_SAMPLER_H_

//Thread
void Sampler_init(int *terminate_flag);

//Cleanup
void Sampler_cleanup(void);

//Join
void Sampler_join(void);

// Getter
int Sampler_getHistorySize(void);
int Sampler_getDips(void);
double Sampler_getAverageReading(void);
long long Sampler_getNumSamplesTaken(void); 
double *Sampler_getHistory(int *size);

#endif