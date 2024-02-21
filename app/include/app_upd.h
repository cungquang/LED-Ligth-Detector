#ifndef _APP_UDP_H_
#define _APP_UDP_H_

// Thread
void UDP_initServer(int *terminate_flag);

//Clean up
void UDP_cleanup(void);

//Join
void UDP_join(void);

#endif