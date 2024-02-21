#ifndef _APP_UDP_H_
#define _APP_UDP_H_

// This module manages UDP server (application level)
// - Initiate the thread to run server - allow user to remotely control the program
// - Join & clean up the program after complete - free all resources after use

// Thread
void UDP_initServer(int *terminate_flag);

//Clean up
void UDP_cleanup(void);

//Join
void UDP_join(void);

#endif