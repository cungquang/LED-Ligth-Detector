#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include "../../hal/include/i2c.h"
#include "../include/app_helper.h"

#define SWITCH_TIME 5

int *isTerminated;

pthread_t i2c_id;
static int leftDigit = 0;
static int rightDigit = 5;

//Initiate function
void displayDigit(int digit);
void *i2c_thread();


void I2C_init(int *terminate_flag)
{
    isTerminated = terminate_flag;
	i2c_init();
    
    //Create thread
    pthread_create(&i2c_id, NULL, i2c_thread, NULL);
}

void I2C_join()
{
    pthread_join(i2c_id, NULL);
}


//////////////////////////////////////////////// PRIVATE ////////////////////////////////////////////////

void *i2c_thread() 
{
    while(!*isTerminated)
    {
        //turn off both digits
        i2c_disableLeftDigit();
        i2c_disableRightDigit();

        //display pattern for left digit
        displayDigit(leftDigit);
        i2c_enableLeftDigit();
        sleepForMs(SWITCH_TIME);

        //turn off both digits
        i2c_disableLeftDigit();
        i2c_disableRightDigit();

        //display pattern for right digit
        displayDigit(rightDigit);
        i2c_enableRightDigit();
        sleepForMs(SWITCH_TIME);
    }

    return NULL;
}

void displayDigit(int digit)
{   
    switch(digit)
    {
        case 0:
            i2c_set0();
            break;
        case 1:
            i2c_set1();
            break;
        case 2:
            i2c_set2();
            break;
        case 3:
            i2c_set3();
            break;
        case 4:
            i2c_set4();
            break;
        case 5:
            i2c_set5();
            break;
        case 6:
            i2c_set6();
            break;
        case 7:
            i2c_set7();
            break;
        case 8:
            i2c_set8();
            break;
        case 9:
            i2c_set9();
            break;
        default:
            perror("Invalid input digit for display");
            exit(1);
    }
}