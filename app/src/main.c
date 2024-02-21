#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>
#include "../include/app_sampler.h"
#include "../include/app_upd.h"
#include "../include/app_helper.h"
#include "../include/app_ledP921.h"
#include "../include/app_i2c.h"
#include "../include/periodTimer.h"


int terminate_flag = 0;

void testLed()
{
	Led_init(&terminate_flag);

	Led_joinThreads();
	Led_cleanUp();
}

void testI2C()
{
	I2C_init(&terminate_flag);

	I2C_join();
}

void testSampler()
{
	UDP_initServer(&terminate_flag);
	SAMPLER_init(&terminate_flag);

	SAMPLER_join();
	UDP_join();

	SAMPLER_cleanup();
	UDP_cleanup();
}

int main() 
{
    // Initialize the module's data structures
    Period_init();

    // Simulate marking events
    for (int i = 0; i < 100; ++i) {
        // Simulate marking PERIOD_EVENT_SAMPLE_LIGHT
        Period_markEvent(PERIOD_EVENT_SAMPLE_LIGHT);
        // Simulate some work here...
    }

    // Get statistics for PERIOD_EVENT_SAMPLE_LIGHT
    Period_statistics_t stats;
    Period_getStatisticsAndClear(PERIOD_EVENT_SAMPLE_LIGHT, &stats);

    // Print statistics
    printf("Statistics for PERIOD_EVENT_SAMPLE_LIGHT:\n");
    printf("Number of Samples: %d\n", stats.numSamples);
    printf("Minimum Period (ms): %lf\n", stats.minPeriodInMs);
    printf("Maximum Period (ms): %lf\n", stats.maxPeriodInMs);
    printf("Average Period (ms): %lf\n", stats.avgPeriodInMs);

    // Cleanup resources
    Period_cleanup();

    return 0;
}
