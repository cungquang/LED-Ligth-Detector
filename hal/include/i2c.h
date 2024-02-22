#ifndef _I2C_H_
#define _I2C_H_

// This module manages the operation for I2C bus (hardware level)
// - Enable digits display
// - Disable digits display
// - Set value/number to display

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
void i2c_set3();
void i2c_set4();
void i2c_set5();
void i2c_set6();
void i2c_set7();
void i2c_set8();
void i2c_set9();

#endif