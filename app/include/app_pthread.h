#ifndef _APP_PTHREAD_H_
#define _APP_PTHREAD_H_

#include <signal.h>

void init_thread(bool terminate_flag);

void handle_shutdown(int signum);


#endif