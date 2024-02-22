#ifndef _HAL_HELPER_H_
#define _HAL_HELPER_H_

// This module manages access to GPIO file (hardware level)
// - Support Write operation to GPIO
// - Support Read operation to GPIO

void writeToFile(const char* fileToWriteGpio, const char* content);
void readFromFile(const char* fileToReadGpio, char* buff, unsigned int maxLength);

#endif