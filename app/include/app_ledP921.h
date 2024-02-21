#ifndef _APP_LEDP921_H_
#define _APP_LEDP921_H_

void LED_init(int *terminate_flag);
void LED_join();
void LED_cleanUp();
int LED_getPot();
int LED_getPotHz();

#endif