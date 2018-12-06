/*
 *Author: Caspian Peavyhouse
 *Assignment Number: A4
 *Date of Submission: 12/07/2018
 *Name of this file: MemoryManager.c
 *Description of the program:
 *Memory Manager - simulates demand paging between the RAM and disk
 */


#include "MemoryManager.h"



/*
 *Function Name: initMemory
 *Input to the method: None
 *Output(Return value): void
 *Description: Allocate space for the memory manager
 */
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

/*
 *Function Name: freeMemory
 *Input to the method: None
 *Output(Return value): void
 *Description: free the memory used by the memory manager
 */
void freeMemory()
{
    free(RAM);
    free(pageTable);
    free(lruTable);
}

/*
 *Function Name: extractUpperByte
 *Input to the method:
 *      uint16_t address - the 16 bit address
 *Output(Return value): uint8_t - the byte extracted
 *Description: extract the upper byte of a 2 byte number
 */
uint8_t extractUpperByte(uint16_t address)
{
    //Shift the register left by one byte
    return (uint8_t) ((address & 0xFF00) >> 8);
}

/*
 *Function Name: extractLowerByte
 *Input to the method:
 *      uint16_t address - the 16 bit address
 *Output(Return value): uint8_t - the byte extracted
 *Description: extract the lower byte of a 2 byte number
 */
uint8_t extractLowerByte(uint16_t address)
{
    //clear the upper byte and return the lower byte
    return (uint8_t) (address & 0x00FF);
}

/*
 *Function Name: getPhysicalAddress
 *Input to the method:
 *      uint16_t logicalAddress - the 16 logical address
 *Output(Return value): uint16_t - physical address for the logical address provided
 *Description: convert a logical address to a physical address
 */
uint16_t getPhysicalAddress(uint16_t logicalAddress)
{
    //Get the page number from the logical address
    uint8_t pageNumber = extractUpperByte(logicalAddress);
    //Get the offset from the logical address
    uint8_t offset = extractLowerByte(logicalAddress);

    //Get the frame number for the page in RAM
    uint8_t frameNumber = extractLowerByte((uint16_t)getFrameNumber(pageNumber));
    //Combine the frame number and the offset
    uint16_t  physicalAddress = (frameNumber << 8) | offset;
    return physicalAddress;
}

/*
 *Function Name: getFrameNumber
 *Input to the method:
 *      uint8_t pageNumber - the 8-bit page number
 *Output(Return value): int - the frame the data is in in the ram
 *Description: Get the frame number of the page in ram
 *  if it isn't in ram, put it in ram
 */
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

/*
 *Function Name: getLeastRecentlyUsedFrame
 *Input to the method: None
 *Output(Return value): int - number of the lru frame
 *Description: Get the least recently used frame number
 */
int getLeastRecentlyUsedFrame()
{
    int smallest = 0;
    int counter;
    //cycle through all the values in the lruArray
    for (counter = 0; counter < FRAME_COUNT; counter++)
    {
        //compare it to the smallest value
        if (lruTable[counter] < lruTable[smallest])
            smallest = counter;
    }

    return smallest;
}

/*
 *Function Name: getPageNumberThatHasFrameNumber
 *Input to the method:
 *      int frameNumber - the frame number to check for
 *Output(Return value): int - the page number found
 *Description: find out which page occupies a frame in ram
 */
int getPageNumberThatHasFrameNumber(int frameNumber)
{
    //Cycle through the page table to find out which page occupies a frame in the ram
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

/*
 *Function Name: readFromRAM
 *Input to the method:
 *      uint16_t physicalAddress - the physical address of the byte in RAM
 *Output(Return value): uint8_t - the byte read from ram
 *Description: Read a byte from memory
 */
uint8_t readFromRAM(uint16_t physicalAddress)
{
    uint8_t frameNumber = extractUpperByte(physicalAddress);
    lruCounter++; //increment the LRU counter

    lruTable[frameNumber] = lruCounter; //update the LRU table

    return RAM[physicalAddress]; //return the data
}

/*
 *Function Name: readFromDisk
 *Input to the method:
 *      uint8_t pageNumber - the page on disk to read
 *Output(Return value): DATAFRAME - the frame read from the disk
 *Description: read a page from disk to copy into ram frame
 */
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

/*
 *Function Name: copyDataFrameToRAM
 *Input to the method:
 *      int frameNumber - the frame number to write to
 *      DATAFRAME dataFrame - the dataframe to write
 *Output(Return value): void
 *Description: write a page from disk to a frame in RAM
 */
void copyDataFrameToRAM(int frameNumber, DATAFRAME dataframe)
{
    //offset to write to the correct frame in ram
    int offset = frameNumber * FRAME_SIZE;
    int current;

    for ( current = 0; current < FRAME_SIZE; current++)
    {
        //write FRAME_SIZE bytes
        RAM[(current + offset)] = dataframe.frame[current];
    }
}
