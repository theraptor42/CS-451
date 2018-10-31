/*
 *Author: Caspian Peavyhouse
 *Assignment Number: A1
 *Date of Submission: 10/17/2018
 *Name of this file: Parser.h
 *Description of the program:
 * 5ps - A mini clone of the ps command
 *     - Provides basic info about a task
 */

#ifndef ASSIGNMENT_1_PARSER_H
#define ASSIGNMENT_1_PARSER_H

#include <stdio.h>  //Need this for the file pointer

//Sizes used to allocate space for the different process info fields
#define MAX_STATE_LENGTH        1
#define MAX_TIME_LENGTH         10
#define MAX_MEMORY_LENGTH       32
#define MAX_COMMAND_LENGTH      255

//Used to allow for modular file retreival
#define STAT_FILE               1
#define STATM_FILE              2
#define CMDLINE_FILE            3

#define STAT_FILE_NAME          "/stat"
#define STATM_FILE_NAME         "/statm"
#define CMDLINE_FILE_NAME       "/cmdline"

//Used for formatting the time output
#define TICKS_IN_SECOND         sysconf(_SC_CLK_TCK)
#define SECONDS_IN_MINUTE       60
#define MINUTES_IN_HOUR         60

//Used for formatting the header line
#define PID_HEADER              "PID"
#define STATE_HEADER            "S"
#define TIME_HEADER             "TIME"
#define MEMORY_HEADER           "SZ"
#define COMMAND_HEADER          "CMD"

//Used to decide how many tabs to include between each data field in the output
#define ONE_TAB                 1
#define TWO_TAB                 2
#define THREE_TAB               3


//main driver of the Parser module
char* parseProcFiles(short optionMask, char* pid, char* pidPath);
//The functions that actually parse the data
int parseState(char* procState, FILE* filePtr);
int parseTime(char* procTime, FILE* filePtr);
int parseMemory(char* procMemory, FILE* filePtr);
int parseCommand(char* procCommand, FILE* filePtr);

//Support functions for getting the data and building the output
char* appendToOutputString(char *currentValue, char *additionalValue, int tabCount);
int formatDateString(long long ticks, char* procTime);
FILE* getFilePointer(char* PidPath, int fileName);
char* concatenateTwoStrings(char *firstString, char *secondString);
char* concatenateFourStrings(char *firstString, char *secondString, char *thirdString, char *fourthString);
int checkFileExists(char* filePath);


#endif //ASSIGNMENT_1_PARSER_H
