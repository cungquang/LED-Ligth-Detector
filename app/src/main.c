#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>
#include "../include/app_sampler.h"
#include "../include/app_upd.h"
#include "../include/app_helper.h"

int terminate_flag;

void operation()
{
	terminate_flag = 0;
	Udp_initServer(&terminate_flag);

	Udp_join();

	Udp_cleanup();
}

int main()
{	
	char buffer[1500];
	int buffer_size = 0;
	double strNum[] = {123.3425, 3431.1234, 23123.3416};

	for(int i = 0; i < 3; i++)
	{
		int char_size;
		const char *str = convertDataToString(&char_size, strNum[i]);
		
		//merge to buffer
		mergeToBuffer(buffer, &buffer_size, str, char_size);
		printf("%d - %s\n", buffer_size, buffer);

		//free memory for the next round
		free((void *) str);
		str = NULL;
	}

	return 0;
}
