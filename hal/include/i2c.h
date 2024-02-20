#ifndef _I2C_H_
#define _I2C_H_

void i2c_init();

//Left digit
void i2c_enableLeftDigit();
void i2c_disableLeftDigit();

//Right digit
void i2c_enableRightDigit();
void i2c_disableRightDigit();

//Set value
void i2c_set0();
void i2c_set1();
void i2c_set2();

#endif