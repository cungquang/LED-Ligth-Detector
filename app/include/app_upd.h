#ifndef _APP_UDP_H_
#define _APP_UDP_H_

// Thread
void Udp_initServer(int *terminate_flag);

//Clean up
void Udp_cleanup(void);

//Join
void Udp_join(void);

const char *command_help(void);

#endif