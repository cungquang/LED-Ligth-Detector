#ifndef _APP_I2C_H_
#define _APP_I2c_H_

// This module manages the operation I2C Bus - display digit (application level)
// - Initiate I2C thread
// - Join the I2C thread
// - Terminate & clean up the module
// - Allow access to the dips data 

void I2C_init(int *terminate_flag);
void I2C_join();
void I2C_setDipsToDisplay(int dipsValue);
void I2C_cleanUp();

#endif