/*
 *Author: Caspian Peavyhouse
 *Assignment Number: A1
 *Date of Submission: 10/17/2018
 *Name of this file: Parser.c
 *Description of the program:
 * 5ps - A mini clone of the ps command
 *     - Provides basic info about a task
 */



#include <stdlib.h>
//#include <stdio.h>
#include <memory.h>
//#include <math.h>
#include <unistd.h>
#include "Parser.h"
#include "OptionsProcessing.h"

/*
 *Function Name: parseProcFile
 *Input to the method: *The options that the user picked
 *                     *The pid of the task being examined
 *                     *The logical path to the pid resources
 *Output(Return value): char* -> The 5ps program output
 *Description: Drives the file parsing and output building of the program
 */
char* parseProcFiles(short optionMask, char* pid, char* pidPath)
{
    //allocate some space for the data fields
    char* procState = (char *)malloc(sizeof(char) * MAX_STATE_LENGTH);
    char* procTime = (char *)malloc(sizeof(char) * MAX_TIME_LENGTH);
    char* procMemory = (char *)malloc(sizeof(char) * MAX_MEMORY_LENGTH);
    char* procCommand = (char *)malloc(sizeof(char) * MAX_COMMAND_LENGTH);

    //declare the output strings
    char* outputHeaderString = "";
    char* outputValueString = "";

    if (optionMask)
    {
        //Add the PID the the output
        outputHeaderString = appendToOutputString(outputHeaderString, PID_HEADER, ONE_TAB);
        outputValueString = appendToOutputString(outputValueString, pid, ONE_TAB);

        //if the -s flag is selected, add the state to the output
        if (optionMask & S_FLAG)
        {
            parseState(procState, getFilePointer(pidPath, STAT_FILE));

            outputHeaderString = appendToOutputString(outputHeaderString, STATE_HEADER, ONE_TAB);
            outputValueString = appendToOutputString(outputValueString, procState, ONE_TAB);
        }
        //if the - flag is selected, add the time info to the output
        if (optionMask & T_FLAG)
        {
            parseTime(procTime, getFilePointer(pidPath, STAT_FILE));

            outputHeaderString = appendToOutputString(outputHeaderString, TIME_HEADER, TWO_TAB);
            outputValueString = appendToOutputString(outputValueString, procTime, TWO_TAB);
        }
        //if the -v flag is selected, add the memory info to the output
        if (optionMask & V_FLAG)
        {
            parseMemory(procMemory, getFilePointer(pidPath, STATM_FILE));

            outputHeaderString = appendToOutputString(outputHeaderString, MEMORY_HEADER, TWO_TAB);
            outputValueString = appendToOutputString(outputValueString, procMemory, TWO_TAB);
        }
        //if the -c flag is selected, add the command to the output
        if (optionMask & C_FLAG)
        {
            parseCommand(procCommand, getFilePointer(pidPath, CMDLINE_FILE));

            outputHeaderString = appendToOutputString(outputHeaderString, COMMAND_HEADER, THREE_TAB);
            outputValueString = appendToOutputString(outputValueString, procCommand, THREE_TAB);
        }
    }
    else
    {
        // if no data options are selected, return nothing
        return NULL;
    }
    //Free up the memory I allocated
    free(procState);
    free(procTime);
    free(procMemory);
    free(procCommand);

    //Return the formatted output
    return concatenateFourStrings(outputHeaderString, "\n", outputValueString, "\n");
}

/*
 *Function Name: appendToOutputString
 *Input to the method: *The pointer to the output field I'm modifying (Header or data)
 *                     *The data I'm adding to the output
 *                     *The number of tabs to put at the end of the output for spacing
 *Output(Return value): char* -> The modified output string
 *Description: Adds a piece of data to the output strings
 */
char* appendToOutputString(char *currentValue, char *additionalValue, int tabCount)
{
    //declare the return variable
    char* newValueLine;
    //switch on the number of tabs you want at the end of the field
    //some of the layout artifacts are from reformatting changes
    switch (tabCount)
    {
        case ONE_TAB:
            newValueLine = concatenateFourStrings(currentValue, "", additionalValue, "\t");
            break;
        case TWO_TAB:
            newValueLine = concatenateFourStrings(currentValue, "", additionalValue, "\t\t");
            break;
        case THREE_TAB:
            newValueLine = concatenateFourStrings(currentValue, "", additionalValue, "\t\t\t");
            break;
        default:
            newValueLine = concatenateFourStrings(currentValue, "", additionalValue, "\t\t");
            break;
    }

    return newValueLine;
}

/*
 *Function Name: parseState
 *Input to the method: *The parsed state
 *                     *The pointer to the stat file
 *Output(Return value): None
 *Description: Parses the State field from the stat file
 */
int parseState(char* procState, FILE* filePtr)
{
    //Ignore an initial integer, and then an intial string, take a string then ignore everything else
    fscanf(filePtr, "%*d %*s %s %*s", procState);
    return 0;
}

/*
 *Function Name: parseTime
 *Input to the method: *The parsed time field
 *                     *The pointer to the stat file
 *Output(Return value): None
 *Description: Parses the utime and stime fields from the stat file
 */
int parseTime(char* procTime, FILE* filePtr)
{
    //allocate some space for the read data
    char *sTimeStr = (char *)malloc(sizeof(char) * MAX_TIME_LENGTH);
    char *uTimeStr = (char *)malloc(sizeof(char) * MAX_TIME_LENGTH);
    char **endPtr = 0;

    //we have to skip a lot of data
    fscanf(filePtr, "%*s %*s %*s %*s %*s %*s %*s %*s %*s %*s %*s %*s %*s %s %s %*s", sTimeStr, uTimeStr);

    long long sTime = strtol(sTimeStr, endPtr, 10);
    long long uTime = strtol(uTimeStr, endPtr, 10);

    //number of ticks elapsed
    long long procTimeNumber = ( sTime + uTime );
    free (sTimeStr);
    free (uTimeStr);

    //format the output
    formatDateString(procTimeNumber, procTime);
    return 0;
}

/*
 *Function Name: parseMemory
 *Input to the method: *The parsed memory information
 *                     *The pointer to the statm file
 *Output(Return value): None
 *Description: Parses the  memory field from the statm file
 */
int parseMemory(char* procMemory, FILE* filePtr)
{
    //Ignore an initial integer, and then an intial string, take a string then ignore everything else
    fscanf(filePtr, "%s %*s", procMemory);
    return 0;
}

/*
 *Function Name: parseCommand
 *Input to the method: *The parsed command line
 *                     *The pointer to the cmdline file
 *Output(Return value): None
 *Description: Parses the command field from the cmdline file
 */
int parseCommand(char* procCommand, FILE* filePtr)
{
    //Ignore an initial integer, and then an intial string, take a string then ignore everything else
    fscanf(filePtr, "%s", procCommand);
    return 0;
}


/*
 *Function Name: formatDateString
 *Input to the method: *The number of ticks elapsed
 *                     *The return formatted time
 *Output(Return value): None
 *Description: Formats the ticks to human readable format
 */
int formatDateString(long long ticks, char* procTime)
{
    //Standard algorithm for formatting date
    long long hours = ticks / ( TICKS_IN_SECOND *  SECONDS_IN_MINUTE * MINUTES_IN_HOUR ) ;
    ticks = ticks % ( TICKS_IN_SECOND *  SECONDS_IN_MINUTE * MINUTES_IN_HOUR );

    long long minutes = (ticks / ( TICKS_IN_SECOND *  SECONDS_IN_MINUTE ));
    ticks = ticks % ( TICKS_IN_SECOND *  SECONDS_IN_MINUTE );

    long long seconds = ticks / ( TICKS_IN_SECOND );

    //Pretty formatting stuff
    int minutesUpper = (int)minutes/10;
    int minutesLower = (int)minutes%10;

    int secondsUpper = (int)seconds/10;
    int secondsLower = (int)seconds%10;

    sprintf(procTime, "%lli:%i%i:%i%i", hours, minutesUpper, minutesLower, secondsUpper, secondsLower);
    return 0;
}


/*
 *Function Name: getFilePointer
 *Input to the method: *The path to the process resources
 *                     *The #Define keyword name of the file
 *Output(Return value): File* -> the file requested
 *Description: Uses a keyword to get a file pointer
 */
FILE* getFilePointer(char* PidPath, int fileName)
{
    char *filePath;
    char *fileNamePtr;
    //Uses #define keywords like an enum
    switch (fileName)
    {
        case STAT_FILE:
            fileNamePtr = STAT_FILE_NAME;
            break;
        case STATM_FILE:
            fileNamePtr = STATM_FILE_NAME;
            break;
        case CMDLINE_FILE:
            fileNamePtr = CMDLINE_FILE_NAME;
            break;
        default:
            printf("Unfortunately, the program has encountered an error and must exit.");
            exit(ERROR_EXIT_STATUS);

    }
    //Join the file name to the path
    filePath = concatenateTwoStrings(PidPath, fileNamePtr);
    //Check if the file exists
    checkFileExists(filePath);
    //get the file pointer
    FILE* returnPtr = fopen(filePath, "r");

    free(filePath);
    return returnPtr;
}

/*
 *Function Name: concatenateTwoStrings
 *Input to the method: *The first string to join
 *                     *The second string to join
 *Output(Return value): char* -> The joined string
 *Description: Concatenates two strings
 */
char* concatenateTwoStrings(char *firstString, char *secondString)
{
    //joins two strings
    char* target = (char *)malloc(sizeof(char) * (strlen(firstString) + strlen(secondString)));
    sprintf(target, "%s%s", firstString, secondString);
    return target;
}

/*
 *Function Name: concatenateFourStrings
 *Input to the method: *The ordered list of strings to join
 *Output(Return value): char* -> The joined string
 *Description: Concatenates four strings
 */
char* concatenateFourStrings(char *firstString, char *secondString, char *thirdString, char *fourthString)
{
    //This was just convenient for the way I wanted to format my output
    char* tempTargetOne;
    char* tempTargetTwo;

    //Like a MergeSort, but ust joining strings
    tempTargetOne = concatenateTwoStrings(firstString, secondString);
    tempTargetTwo = concatenateTwoStrings(thirdString, fourthString);
    char* target = concatenateTwoStrings(tempTargetOne, tempTargetTwo);

    free(tempTargetOne);
    free(tempTargetTwo);

    return target;
}

/*
 *Function Name: checkFileExists
 *Input to the method: *The path to the file I'm checking
 *Output(Return value): None
 *Description: Checks if a file exists
 */
int checkFileExists(char *filePath)
{
    if(access(filePath, F_OK ) != -1 )
    {
        return 0;
    }
    else
    {
        //if the file doesn't exist, exit
        printf("The specified file does not exist: %s\n", filePath);
        exit(ERROR_EXIT_STATUS);
    }
}
