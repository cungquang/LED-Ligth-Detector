#ifndef _APP_SHUTDOWNP_H_
#define _APP_SHUTDOWNP_H_

//This module manages the shutdown operation (application level)
// - Initiate the thread of shutdown - check and clean up all the resources
// - Join the thread after complete

void SHUTDOWN_init(int *cleanUpFlag);
void SHUTDOWN_join();

#endif