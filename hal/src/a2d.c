#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>

#define A2D_FILE_VOLTAGE0 "/sys/bus/iio/devices/iio:device0/in_voltage0_raw"
#define A2D_FILE_VOLTAGE1 "/sys/bus/iio/devices/iio:device0/in_voltage1_raw"
#define A2D_VOLTAGE_REF_V 1.8
#define A2D_MAX_READING 4095

static FILE *fileToRead;

//Initiate function
int A2D_readRawVoltage(const char *a2dFilePath);

void closeFile() 
{
    if(fileToRead)
    {
        fclose(fileToRead);
    }
}

int A2D_readFromVoltage0()
{
    return A2D_readRawVoltage(A2D_FILE_VOLTAGE0);
}

int A2D_readFromVoltage1()
{
    return A2D_readRawVoltage(A2D_FILE_VOLTAGE1);
}

double A2D_convertVoltage(int reading)
{
    double voltage = ((double)reading / A2D_MAX_READING) * A2D_VOLTAGE_REF_V;
    return voltage;
}

/////////////////////////////////////////////// PRIVATE ///////////////////////////////////////////////

int A2D_readRawVoltage(const char *a2dFilePath)
{
    fileToRead = fopen(a2dFilePath, "r");
    if (!fileToRead) {
        printf("ERROR: Unable to open voltage input file. Cape loaded?\n");
        printf(" Check /boot/uEnv.txt for correct options.\n");
        exit(-1);
    }

    // Get reading
    int a2dReading = 0;
    int itemsRead = fscanf(fileToRead, "%d", &a2dReading);
    if (itemsRead <= 0) {
        printf("ERROR: Unable to read values from voltage input file.\n");
        exit(-1);
    }

    // Close file
    fclose(fileToRead);
    return a2dReading;
}