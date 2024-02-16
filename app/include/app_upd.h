#ifndef _APP_UDP_H_
#define _APP_UDP_H_

void Udp_initServer(bool terminate_flag);
void Udp_initClient(bool terminate_flag);
void Udp_cleanup();

// Setter
void Udp_setTerminate(bool terminate_flag);

#endif