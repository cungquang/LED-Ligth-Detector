#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>

#define A2D_FILE_VOLTAGE0 "/sys/bus/iio/devices/iio:device0/in_voltage0_raw"
#define A2D_VOLTAGE_REF_V 1.8
#define A2D_MAX_READING 4095

static FILE *fileToRead;

int getVoltage0Read()
{
    fileToRead = fopen(A2D_FILE_VOLTAGE0, "r");
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
    if(fileToRead){
        fclose(fileToRead);
    }
    return a2dReading;
}

double getVoltageConvert(int reading)
{
    double voltage = ((double)reading / A2D_MAX_READING) * A2D_VOLTAGE_REF_V;
    return voltage;
}

void closeFile() {
    if(fileToRead)
    {
        fclose(fileToRead);
    }
}