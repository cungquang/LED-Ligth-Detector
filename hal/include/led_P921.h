#ifndef _LED_P921_H_
#define _LED_P921_H_

// This modules monitors the operation of the led light (hardware level)
// - Support to configure/enable file for led management
// - Set the period (operation) for the led
// - Set the duty_cycle of the led
// - Enable/Disable the led

void led_configure();
int led_getPeriod();
int led_getDutyCycle();
void led_writeToEnable(int value);
void led_writeToDutyCycle(int value);
void led_writeToPeriod(int value);

#endif