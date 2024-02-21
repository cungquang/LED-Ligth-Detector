#ifndef _A2D_H_
#define _A2D_H_

// This module manages the access to A2D (hardware level) file
// - Support read from voltage0
// - Support read from voltage1
// - Support convert raw data to voltage value

int a2d_readFromVoltage0();
int a2d_readFromVoltage1();
double a2d_convertVoltage(int reading);

#endif