#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>

#define A2D_FILE_VOLTAGE0 "/sys/bus/iio/devices/iio:device0/in_voltage0_raw"
#define A2D_FILE_VOLTAGE1 "/sys/bus/iio/devices/iio:device0/in_voltage1_raw"
#define A2D_VOLTAGE_REF_V 1.8
#define A2D_MAX_READING 4095

static FILE *voltage0File;
static FILE *voltage1File;

int A2D_readFromVoltage0()
{
    voltage0File = fopen(A2D_FILE_VOLTAGE0, "r");
    if (!voltage0File) {
        printf("ERROR: Unable to open voltage input file. Cape loaded?\n");
        printf(" Check /boot/uEnv.txt for correct options.\n");
        exit(-1);
    }

    // Get reading
    int a2dReading = 0;
    int itemsRead = fscanf(voltage0File, "%d", &a2dReading);
    if (itemsRead <= 0) {
        printf("ERROR: Unable to read values from voltage input file.\n");
        exit(-1);
    }

    // Close file
    fclose(voltage0File);
    return a2dReading;
}

int A2D_readFromVoltage1()
{
    voltage1File = fopen(A2D_FILE_VOLTAGE1, "r");
    if (!voltage1File) {
        printf("ERROR: Unable to open voltage input file. Cape loaded?\n");
        printf(" Check /boot/uEnv.txt for correct options.\n");
        exit(-1);
    }

    // Get reading
    int a2dReading = 0;
    int itemsRead = fscanf(voltage1File, "%d", &a2dReading);
    if (itemsRead <= 0) {
        printf("ERROR: Unable to read values from voltage input file.\n");
        exit(-1);
    }

    // Close file
    fclose(voltage1File);
    return a2dReading;
}

double A2D_convertVoltage(int reading)
{
    double voltage = ((double)reading / A2D_MAX_READING) * A2D_VOLTAGE_REF_V;
    return voltage;
}