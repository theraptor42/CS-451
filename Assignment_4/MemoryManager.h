/*
 *Author: Caspian Peavyhouse
 *Assignment Number: A4
 *Date of Submission: 12/07/2018
 *Name of this file: MemoryManager.h
 *Description of the program:
 *Memory Manager - simulates demand paging between the RAM and disk
 */


#ifndef ASSIGNMENT_4_MEMORYMANAGER_H
#define ASSIGNMENT_4_MEMORYMANAGER_H


//includes
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h> //Used by memset


//defines
#define FRAME_SIZE          256 //2^8 bytes
#define PAGE_SIZE           256 //2^8
#define PAGE_COUNT          256
#define FRAME_COUNT         64 //2^6 frames
#define DISK_FILE           "./BACKING_STORE.bin"

//64 Frames * 256 Bytes per frame  * 1 byte per data block
#define SIZE_OF_RAM         (sizeof(uint8_t) * FRAME_COUNT * FRAME_SIZE)
//256 pages to keep track of * 1 signed int per page
#define SIZE_OF_PAGE_TABLE  (sizeof(int) * PAGE_COUNT)
//64 Frames * 1 long unsigned int each
#define SIZE_OF_LRU         (sizeof(long unsigned int) * FRAME_COUNT)

//Struct Declaration
struct DataFrame
{
    //signed char a[10];
    uint8_t  frame[PAGE_SIZE];
};
typedef struct DataFrame DATAFRAME;

//global variables
uint8_t* RAM; //[FRAME_COUNT * FRAME_SIZE];
int* pageTable; //[PAGE_COUNT];
long unsigned int* lruTable; //[FRAME_COUNT];
long unsigned int lruCounter; //number of memory reads, most current memory reads
long unsigned int pageFaults; //number of page faults
int pageFault; //did the current read pagefault


//function prototypes
void initMemory();
void freeMemory();
uint8_t extractUpperByte(uint16_t address);
uint8_t extractLowerByte(uint16_t address);
uint16_t getPhysicalAddress(uint16_t logicalAddress);
int getFrameNumber(uint8_t pageNumber);
int getLeastRecentlyUsedFrame();
int getPageNumberThatHasFrameNumber(int frameNumber);


uint8_t readFromRAM(uint16_t physicalAddress);
DATAFRAME readFromDisk(uint8_t pageNumber);
void copyDataFrameToRAM(int frameNumber, DATAFRAME dataframe);


#endif //ASSIGNMENT_4_MEMORYMANAGER_H
