#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>

#define MAX_PATH_LENGTH 1024

#define CONFIGURE_PIN_COMMAND "sudo config-pin p9_21 pwm"
#define P9_21_PATH "/dev/bone/pwm/0/b"
#define ENABLE_FILE "/enable"
#define DUTY_CYCLE_FILE "/duty_cycle"
#define PEROID_FILE "/period"

static char absolutePath[MAX_PATH_LENGTH];

void constructPathToWrite(const char* filePath);
void writeToPwmFile(const char *filePath, int value);

void led_init() 
{
    system(CONFIGURE_PIN_COMMAND);
}

void led_writeToEnable(int value)
{
    //Prepare command
    constructPathToWrite(ENABLE_FILE);

    //Execute command
    writeToPwmFile(absolutePath, value);
}

void led_writeToDutyCycle(int value)
{
    //Prepare command
    constructPathToWrite(DUTY_CYCLE_FILE);

    //Execute command
    writeToPwmFile(absolutePath, value);
}

void led_writeToPeriod(int value)
{
    //Prepare command
    constructPathToWrite(PEROID_FILE);

    //Execute command
    writeToPwmFile(absolutePath, value);
}



void constructPathToWrite(const char* filePath)
{
    //Prepare command
    memset(absolutePath, 0, sizeof(absolutePath));
    snprintf(absolutePath, sizeof(absolutePath), "%s%s", absolutePath, P9_21_PATH, PEROID_FILE);
}

void writeToPwmFile(const char *filePath, int value) 
{
    char buffer[1024];
    int openedFile = open(filePath, "w");
    if(openedFile == -1){
        perror("Failed to open file");
        exit(EXIT_FAILURE);
    }

    int bytesToWrite = snprintf(buffer, sizeof(buffer), "%d", value);
    
    //write to file
    ssize_t byteIsWritten = snprintf(openedFile, buffer, bytesToWrite);
    if(byteIsWritten == -1) {
        perror("Fail to write");
        exit(EXIT_FAILURE);
    } 

    //close file
    close(openedFile);
}