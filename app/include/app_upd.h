#ifndef _APP_UDP_H_
#define _APP_UDP_H_

// Thread
void Udp_initServer(bool terminate_flag);
void Udp_initClient(bool terminate_flag);
void Udp_joinServer();
void Udp_joinClient();

//Clean up
void Udp_cleanup();

// Setter
void Udp_setTerminate(bool terminate_flag);

#endif