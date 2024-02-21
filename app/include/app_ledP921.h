#ifndef _APP_LEDP921_H_
#define _APP_LEDP921_H_

// This module controls the operation of the led (application level)
// - Init led thread to start trigger operation of the led
// - join the thread after complete operation
// - terminate & clean up the module 
// - allow access to raw data of Pot & Hz (convert from pot)

void LED_init(int *terminate_flag);
void LED_join();
void LED_cleanUp();
int LED_getPot();
int LED_getPotHz();

#endif