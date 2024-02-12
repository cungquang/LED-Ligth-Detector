#ifndef _HAL_HELPER_H
#define _HAL_HELPER_H

void writeToFile(const char* fileToWriteGpio, const char* content);
void readFromFile(const char* fileToReadGpio, char* buff, unsigned int maxLength);

#endif