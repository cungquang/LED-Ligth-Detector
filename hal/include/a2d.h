#ifndef _A2D_H_
#define _A2D_H_

// This module manages the access to A2D (hardware level) file
// - Support read from voltage0
// - Support read from voltage1
// - Support convert raw data to voltage value

int A2D_readFromVoltage0();
int A2D_readFromVoltage1();
double A2D_convertVoltage(int reading);

#endif