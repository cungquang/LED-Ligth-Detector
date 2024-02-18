#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>
#include "../include/app_sampler.h"
#include "../include/app_upd.h"
#include "../include/app_helper.h"

#define MAX_BUFFER_SIZE 26

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
	int temp_size;
	const char *str;
	char command_buffer[MAX_BUFFER_SIZE];
	int current_buffer_size = 0;
	double strNum[] = {123.3425, 3431.1234, 23123.3416};

	for(int i = 0; i < 3; i++)
	{
		temp_size = 0;
		str = convertDataToString(&temp_size, strNum[i]);
		
		//if fit into current size - need to + 1 for ','
        if(current_buffer_size + temp_size + 1 < MAX_BUFFER_SIZE)
        {
            mergeToBuffer(command_buffer, &current_buffer_size, str, temp_size);
        }
        //if oversize -> send data
        else
        {
            //need to cast type (struct sockaddr *) for client_addr
            printf("%s\n", command_buffer);

            //reset data
            memset(command_buffer, 0, sizeof(command_buffer));
            current_buffer_size = 0;

			//merge to buffer 
			mergeToBuffer(command_buffer, &current_buffer_size, str, temp_size);
        }

		//free memory for the next round
		free((void *) str);
		str = NULL;
	}

	//need to cast type (struct sockaddr *) for client_addr
    printf("%s\n", command_buffer);
	return 0;
}
