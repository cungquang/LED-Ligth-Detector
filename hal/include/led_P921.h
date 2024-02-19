#ifndef _LED_P921_H_
#define _LED_P921_H_

void led_configure();
int led_getPeriod();
int led_getDutyCycle();
void led_writeToEnable(int value);
void led_writeToDutyCycle(int value);
void led_writeToPeriod(int value);

#endif