#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
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

//Bus address
#define BUS_ADDR "0x20"

//Command to set digit pattern
#define I2C_SET_COMMAND "i2cset -y 1 0x20"

//Register address & value for each number display
#define OXOO_0 "0x00 0xd0"
#define OXO1_0 "0x01 0xa1"
#define OXOO_1 "0x00 0x00"
#define OXO1_1 "0x01 0xe0"
#define OXOO_2 "0x00 0x98"
#define OXO1_2 "0x01 0x83"


//command string
static char command[500];

static void constructCommand(const char bus_addr, const char register_value);

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
    constructCommand(BUS_ADDR, OXOO_0);
    system(command);
    constructCommand(BUS_ADDR, OXO1_0);
    system(command);
}

void i2c_set1()
{
    constructCommand(BUS_ADDR, OXOO_1);
    system(command);
    constructCommand(BUS_ADDR, OXO1_1);
    system(command);
}

void i2c_set2()
{
    constructCommand(BUS_ADDR, OXOO_2);
    system(command);
    constructCommand(BUS_ADDR, OXO1_2);
    system(command);
}


////////////////////////////////////////// PRIVATE ////////////////////////////////////////// 

void constructCommand(const char bus_addr, const char register_value)
{
    //Prepare command
    memset(command, 0, sizeof(command));
    snprintf(command, sizeof(command), "%s %s", bus_addr, register_value);
}
