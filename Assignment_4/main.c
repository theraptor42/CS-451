//
// Created by caspi on 12/5/2018.
//

#include <stdio.h>
#include "MemoryManager.h"

#define STDIN_BUFFER_SIZE       100
//Function prototype
int exitVector(int status);


int main(int argc, char **argv)
{
    if ( argc != 2 )
    {
        //The program was called incorrectly
        printf( "Program Usage: %s ./filename or %s /path/filename\n", argv[0], argv[0]);
        exit(1);
    }
    //initialize the memory manager and allocate space for it
    initMemory();
    //open the input file
    FILE *input = fopen(argv[1],"r"); //Assume arg is of form ./filename

    char buffer[STDIN_BUFFER_SIZE];
    char* result;
    int processCount = 0;
    //build the passengers array

    //printf("Logical address:\tPage Number\tOffset:\tFrame Number:\tPage Fault:\tValue:\tFrame:0\tFrame:1\tFrame:2\tFrame:3\n");

    while((result = fgets(buffer, STDIN_BUFFER_SIZE , input)) != NULL && result != '\0')
    {
        pageFault = 0;
        //read the logical address from the file
        uint16_t logicalAddress;
        sscanf(result, "%d", (int*)&logicalAddress);

        //get the physical address from the logical address
        uint16_t physicalAddress = getPhysicalAddress(logicalAddress);

        //read the data from the ram using the physical address
        int8_t data = readFromRAM(physicalAddress);

        /* Verification / Debug8 */
        uint8_t pageNumber = extractUpperByte(logicalAddress);
        uint8_t frameNumber = extractUpperByte(physicalAddress);
        uint8_t offset = extractLowerByte(logicalAddress);


         /**/
        //Pretty print the output
        printf("Logical address:\t%5d\tPhysical address:\t%5d\tValue:\t%d\n", logicalAddress, physicalAddress, data);
        //printf("Logical address:\t%04x\tPage Number:\t%02x\tOffset:\t%02x\tPhysical address:\t%04x\tFrame Number:\t%02x\tPage Fault:\t%1d\tValue:\t%4d\n",
            //logicalAddress, pageNumber, offset, physicalAddress, frameNumber, pageFault, data);
        //The following was used for generating the portion of output in the DEMO.txt
        //printf("%04x\t%02x\t%02x\t%02x\t%d\t%5d\t%02x\t%02x\t%02x\t%02x\n",
        //        logicalAddress, pageNumber, offset, frameNumber, pageFault, data, getPageNumberThatHasFrameNumber(0),
        //        getPageNumberThatHasFrameNumber(1),getPageNumberThatHasFrameNumber(2),getPageNumberThatHasFrameNumber(3));
    }

    printf("\nPage faults: %li\tMemory Accesses: %li\tPage Fault Rate: %f\n",
            pageFaults, lruCounter, (double)((long double)pageFaults/lruCounter));
    exitVector(0);
}

int exitVector(int status)
{
    freeMemory();
    exit(status);
}