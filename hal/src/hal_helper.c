#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

void writeToFile(const char* fileToWriteGpio, const char* content) {
    FILE *pFile = fopen(fileToWriteGpio, "w");
    if (pFile == NULL){
        printf("Error to open the file %s\n", fileToWriteGpio); 
        exit(EXIT_FAILURE);
    }
    fprintf(pFile, "%s", content);
    fclose(pFile);
}

void readFromFile(const char* fileToReadGpio, char* buff, unsigned int maxLength){
    FILE *pFile = fopen(fileToReadGpio, "r");
    if (pFile == NULL) {
        printf("Error to open the file %s\n", fileToReadGpio); 
        exit(EXIT_FAILURE);
    }

	// Use fgets to read a line into the provided buffer
    if (fgets(buff, maxLength, pFile) == NULL) {
        fclose(pFile);
        exit(EXIT_FAILURE);
    }

    fclose(pFile);
}