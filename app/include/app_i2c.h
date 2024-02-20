#ifndef _APP_I2C_H_
#define _APP_I2c_H_

void I2C_init(int *terminate_flag);
void I2C_join();
void I2C_setDipsToDisplay(int dipsValue);

#endif