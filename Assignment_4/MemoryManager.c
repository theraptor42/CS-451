//
// Created by caspi on 12/5/2018.
//


#include "MemoryManager.h"



void initMemory()
{
    //Allocate space for everything
    RAM = (uint8_t *) malloc(SIZE_OF_RAM); //physical memory
    pageTable = (int *) malloc(SIZE_OF_PAGE_TABLE); //It will hold #PAGE_COUNT ints
    lruTable =  (long unsigned int*) malloc(SIZE_OF_LRU); //can get very big

    //clear any existing values
    memset(RAM, 0, SIZE_OF_RAM);
    memset(lruTable, 0, SIZE_OF_LRU);

    //initialize page table to all -1
    memset(pageTable, -1, SIZE_OF_PAGE_TABLE);

    lruCounter = 0;
    pageFaults = 0;
    pageFault = 0;
}

void freeMemory()
{
    free(RAM);
    free(pageTable);
    free(lruTable);
}

uint8_t extractUpperByte(uint16_t address)
{
    return (uint8_t) ((address & 0xFF00) >> 8);
}

uint8_t extractLowerByte(uint16_t address)
{
    return (uint8_t) (address & 0x00FF);
}

uint16_t getPhysicalAddress(uint16_t logicalAddress)
{
    uint8_t pageNumber = extractUpperByte(logicalAddress);
    uint8_t offset = extractLowerByte(logicalAddress);

    uint8_t frameNumber = extractLowerByte((uint16_t)getFrameNumber(pageNumber));
    uint16_t  physicalAddress = (frameNumber << 8) | offset;
    return physicalAddress;

}

int getFrameNumber(uint8_t pageNumber)
{
    //Check if the frameNumber is in the pageTable
    if (pageTable[pageNumber] == -1)
    {
        //If no, we need to copy the frame from the bin file
        //Find Where it goes in the RAM
        int lruFrameNumber = getLeastRecentlyUsedFrame();
        //Clear the previous page table value that had that frame space

        int lruPageInFrameTable = getPageNumberThatHasFrameNumber(lruFrameNumber);
        if (lruPageInFrameTable >= 0)
            pageTable[lruPageInFrameTable] = -1;

        //set the page table to have this frame number
        pageTable[pageNumber] = lruFrameNumber;

        //Get the data frame to copy over
        DATAFRAME frame = readFromDisk(pageNumber);

        //copy the newDataFrame over
        copyDataFrameToRAM(lruFrameNumber, frame);

        //This is a page fault
        pageFaults += 1;
        pageFault = 1;
    }

    //And now return the frame number
    return pageTable[pageNumber];
}

int getLeastRecentlyUsedFrame()
{
    int smallest = 0;
    int counter;
    for (counter = 0; counter < FRAME_COUNT; counter++)
    {
        if (lruTable[counter] < lruTable[smallest])
            smallest = counter;
    }

    return smallest;
}

int getPageNumberThatHasFrameNumber(int frameNumber)
{
    int counter;
    for (counter = 0; counter < PAGE_COUNT; counter++)
    {
        if (pageTable[counter] == frameNumber)
        {
            return counter;
        }
    }
    return -1;
}

uint8_t readFromRAM(uint16_t physicalAddress)
{
    uint8_t frameNumber = extractUpperByte(physicalAddress);
    lruCounter++; //increment the LRU counter

    lruTable[frameNumber] = lruCounter; //update the LRU table

    return RAM[physicalAddress]; //return the data
}


DATAFRAME readFromDisk(uint8_t pageNumber)
{
    FILE* fptr;
    struct DataFrame dataFrame;
    int start = pageNumber * PAGE_SIZE;

    fptr = fopen(DISK_FILE, "r");
    fseek(fptr, start, SEEK_SET);
    fread(&dataFrame, sizeof(struct DataFrame),1,fptr);

    return dataFrame;
}


void copyDataFrameToRAM(int frameNumber, DATAFRAME dataframe)
{
    int offset = frameNumber * FRAME_SIZE;
    int current;
    for ( current = 0; current < FRAME_SIZE; current++)
    {
        RAM[(current + offset)] = dataframe.frame[current];
    }
}
