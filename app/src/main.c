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
	double test = 1234.22343;
	double test2 = 4342.23123;
	int char_size = 0;
	int char_size2 = 0;
	const char *str1 = convertDataToString(&char_size, test);
	const char *str2 = convertDataToString(&char_size2, test2);
	printf("%d - %s\n", char_size, str1);
	printf("%d - %s\n", char_size2, str2);

	mergeToBuffer(buffer, &buffer_size, str1, char_size);
	printf("%d - %s\n", buffer_size, buffer);

	mergeToBuffer(buffer, &buffer_size, str2, char_size2);
	printf("%d - %s\n", buffer_size, buffer);

	free((void *)str1);
	free((void *)str2);
	str1 = NULL;
	str2 = NULL;
	return 0;
}
