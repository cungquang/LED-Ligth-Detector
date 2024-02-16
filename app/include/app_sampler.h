#ifndef _APP_SAMPLER_H_
#define _APP_SAMPLER_H_

void Sampler_init(bool terminate_flag);
void Sampler_cleanup();

// Setter
void Sampler_setTerminate(bool terminate_flag);

// Getter
int Sampler_getHistorySize(void);
double Sampler_getAverageReading(void);
long long Sampler_getNumSamplesTaken(void); 
//double *Sampler_getHistory(int *size);

#endif