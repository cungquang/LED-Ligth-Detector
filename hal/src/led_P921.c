#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

#define MAX_PATH_LENGTH 1024

#define CONFIGURE_PIN_COMMAND "echo \"temppwd\" | sudo -S config-pin p9_21 pwm"
#define P9_21_PATH "/dev/bone/pwm/0/b"
#define ENABLE_FILE "/enable"
#define DUTY_CYCLE_FILE "/duty_cycle"
#define PEROID_FILE "/period"
#define DESIRED_FREQUENCY 500           //Hz

static char absolutePath[MAX_PATH_LENGTH];
static int period;
static int dutyCycle;

void constructPathToWrite(const char* filePath);
void writeToPwmFile(int value);

int led_getPeriod()
{
    return period;
}

int led_getDutyCycle()
{
    return dutyCycle;
}

void led_writeToEnable(int value)
{
    //Prepare command
    constructPathToWrite(ENABLE_FILE);

    //Execute command
    writeToPwmFile(value);
}

void led_writeToDutyCycle(int value)
{
    //Prepare command
    constructPathToWrite(DUTY_CYCLE_FILE);

    //Execute command
    writeToPwmFile(value);
}

void led_writeToPeriod(int value)
{
    //Prepare command
    constructPathToWrite(PEROID_FILE);

    //Execute command
    writeToPwmFile(value);
}

void led_configure() 
{
    system(CONFIGURE_PIN_COMMAND);
    period = (int)(1000000000/DESIRED_FREQUENCY);
    dutyCycle = (int)period/2;
}


//////////////////////////////////////// PRIVATE ////////////////////////////////////////

void constructPathToWrite(const char* filePath)
{
    //Prepare command
    memset(absolutePath, 0, sizeof(absolutePath));
    snprintf(absolutePath, sizeof(absolutePath), "%s%s", P9_21_PATH, filePath);
}

void writeToPwmFile(int value) 
{
    char buffer[1024];
    int openedFile = open(absolutePath, O_WRONLY);
    if(openedFile == -1){
        fprintf(stderr, "Failed to open %s: %s\n", absolutePath, strerror(errno));
        exit(EXIT_FAILURE);
    }

    int bytesToWrite = snprintf(buffer, sizeof(buffer), "%d", value);
    
    //write to file
    ssize_t byteIsWritten = write(openedFile, buffer, bytesToWrite);
    if(byteIsWritten == -1) {
        fprintf(stderr, "Failed to write to %s: %s\n", absolutePath, strerror(errno));
        close(openedFile);
        exit(EXIT_FAILURE);
    } 

    //close file
    close(openedFile);
}