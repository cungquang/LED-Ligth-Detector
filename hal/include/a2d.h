#ifndef _A2D_H_
#define _A2D_H_

void closeFile();
int A2D_readFromVoltage0();
int A2D_readFromVoltage1();
double A2D_convertVoltage(int reading);

#endif