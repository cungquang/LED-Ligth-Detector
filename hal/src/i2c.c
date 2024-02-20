#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>

//Configure command
#define CONFIGURE_PIN_18 "config-pin p9_18 i2c"
#define CONFIGURE_PIN_17 "config-pin p9_17 i2c"

//GPIO61 - right digit
#define DIRECTION_61_OUT "echo out > /sys/class/gpio/gpio61/direction"
#define ENABLE_61 "echo 1 > /sys/class/gpio/gpio61/value"
#define DISABLE_61 "echo 0 > /sys/class/gpio/gpio61/value"

//GPIO44 - left digit
#define DIRECTION_44_OUT "echo out > /sys/class/gpio/gpio44/direction"
#define ENABLE_44 "echo 1 > /sys/class/gpio/gpio44/value"
#define DISABLE_44 "echo 0 > /sys/class/gpio/gpio44/value"

//Command to set digit pattern
#define I2C_SET_COMMAND "i2cset -y 1 0x20"

//Register address & value for each number display from 0 - 9
#define OXOO_0 "0x00 0xd0"
#define OXO1_0 "0x01 0xa1"
#define OXOO_1 "0x00 0x00"
#define OXO1_1 "0x01 0xe0"
#define OXOO_2 "0x00 0x98"
#define OXO1_2 "0x01 0x83"
#define OXOO_3 "0x00 0x18"
#define OXO1_3 "0x01 0xa3"
#define OXOO_4 "0x00 0x48"
#define OXO1_4 "0x01 0xe2"
#define OXOO_5 "0x00 0x58"
#define OXO1_5 "0x01 0x23"
#define OXOO_6 "0x00 0xd8"
#define OXO1_6 "0x01 0x23"
#define OXOO_7 "0x00 0x10"
#define OXO1_7 "0x01 0xe2"
#define OXOO_8 "0x00 0xd8"
#define OXO1_8 "0x01 0xa3"
#define OXOO_9 "0x00 0x58"
#define OXO1_9 "0x01 0xa3"

//Initiate function
void constructCommand(char command[], const char *bus_addr, const char *register_value);

void i2c_init() 
{
    system(CONFIGURE_PIN_17);
    system(CONFIGURE_PIN_18);
    system(DIRECTION_44_OUT);
    system(DIRECTION_61_OUT); 
}

void i2c_enableLeftDigit()
{
    system(ENABLE_44);
}

void i2c_enableRightDigit()
{
    system(ENABLE_61);
}

void i2c_disableLeftDigit()
{
    system(DISABLE_44);
}

void i2c_disableRightDigit()
{
    system(DISABLE_61);
}

void i2c_set0() 
{
    char command_1[500];
    char command_2[500]; 
    constructCommand(command_1, I2C_SET_COMMAND, OXOO_0);
    constructCommand(command_2, I2C_SET_COMMAND, OXO1_0);
    system(command_1);
    system(command_2);
}

//Display number 1
void i2c_set1()
{
    char command_1[500];
    char command_2[500]; 
    constructCommand(command_1, I2C_SET_COMMAND, OXOO_1);
    constructCommand(command_2, I2C_SET_COMMAND, OXO1_1);
    system(command_1);
    system(command_2);
}

//Display number 2
void i2c_set2()
{
    char command_1[500];
    char command_2[500]; 
    constructCommand(command_1, I2C_SET_COMMAND, OXOO_2);
    constructCommand(command_2, I2C_SET_COMMAND, OXO1_2);
    system(command_1);
    system(command_2);
}

void i2c_set3()
{
    char command_1[500];
    char command_2[500]; 
    constructCommand(command_1, I2C_SET_COMMAND, OXOO_3);
    constructCommand(command_2, I2C_SET_COMMAND, OXO1_3);
    system(command_1);
    system(command_2);
}

void i2c_set4()
{
    char command_1[500];
    char command_2[500]; 
    constructCommand(command_1, I2C_SET_COMMAND, OXOO_4);
    constructCommand(command_2, I2C_SET_COMMAND, OXO1_4);
    system(command_1);
    system(command_2);
}

void i2c_set5()
{
    char command_1[500];
    char command_2[500]; 
    constructCommand(command_1, I2C_SET_COMMAND, OXOO_5);
    constructCommand(command_2, I2C_SET_COMMAND, OXO1_5);
    system(command_1);
    system(command_2);
}

void i2c_set6()
{
    char command_1[500];
    char command_2[500]; 
    constructCommand(command_1, I2C_SET_COMMAND, OXOO_6);
    constructCommand(command_2, I2C_SET_COMMAND, OXO1_6);
    system(command_1);
    system(command_2);
}

void i2c_set7()
{
    char command_1[500];
    char command_2[500]; 
    constructCommand(command_1, I2C_SET_COMMAND, OXOO_7);
    constructCommand(command_2, I2C_SET_COMMAND, OXO1_7);
    system(command_1);
    system(command_2);
}

void i2c_set8()
{
    char command_1[500];
    char command_2[500]; 
    constructCommand(command_1, I2C_SET_COMMAND, OXOO_8);
    constructCommand(command_2, I2C_SET_COMMAND, OXO1_8);
    system(command_1);
    system(command_2);
}

void i2c_set9()
{
    char command_1[500];
    char command_2[500]; 
    constructCommand(command_1, I2C_SET_COMMAND, OXOO_9);
    constructCommand(command_2, I2C_SET_COMMAND, OXO1_9);
    system(command_1);
    system(command_2);
}



////////////////////////////////////////// PRIVATE ////////////////////////////////////////// 

void constructCommand(char command[], const char *bus_addr, const char *register_value)
{
    //Prepare command
    memset(command, 0, sizeof(command));
    snprintf(command, sizeof(command), "%s %s", bus_addr, register_value);
}
