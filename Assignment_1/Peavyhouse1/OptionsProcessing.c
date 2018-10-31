/*
 *Author: Caspian Peavyhouse
 *Assignment Number: A1
 *Date of Submission: 10/17/2018
 *Name of this file: OptionsProcessing.c
 *Description of the program:
 * OptionsProcessing - a module for processing
 *          the command line arguments for 5ps
 */

#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <memory.h>
#include <errno.h>
#include "OptionsProcessing.h"


/*
 *Function Name: readOptions
 *Input to the method: *The args being checked
 *                     *Pointer to the PID path
 *                     *Pointer to the PID string
 *Output(Return value): short -> the option mask
 *Description: Reads the input arguments and PID
 */
unsigned short readOptions(int   argc, char **argv, char *taskPidPath, char *taskPid)
{
    unsigned short optionMask = 0;
    int getOptStatus;

    //set the default PID to 1
    strcpy(taskPid, DEFAULT_PID);

    //read the option string
    while ((getOptStatus = getopt (argc, argv, "p:cstv")) != -1)
    {
        switch (getOptStatus)
        {
            case 'c':
                optionMask = optionMask | (unsigned short)C_FLAG;
                break;
            case 's':
                optionMask = optionMask | (unsigned short)S_FLAG;
                break;
            case 't':
                optionMask = optionMask | (unsigned short)T_FLAG;
                break;
            case 'v':
                optionMask = optionMask | (unsigned short)V_FLAG;
                break;
            case 'p':
                strcpy(taskPid, optarg);

                break;
            //This is what it does when it doesn't recognize a character
            case '?':
                if (optopt == 'p')
                {
                    //The -p flag was set but no argument was provided
                    printf("You must provide a PID with the -%c option.\n", optopt);
                    exit(ERROR_EXIT_STATUS);
                }
                else
                {
                    printf("Unknown option `-%c'.\n", optopt);
                    exit(ERROR_EXIT_STATUS);
                }

            default:
                exit(ERROR_EXIT_STATUS);
        }
    }

    //After we check if the user wants a different PID, generate a path for it
    generateDirPath(taskPid, taskPidPath);
    //make sure the directory exists
    processDirExists(taskPidPath);
    //return the other options selected
    return optionMask;
}

/*
 *Function Name: generateDirPath
 *Input to the method: *The process pid
 *                     *The pointer to the output pid path
 *Output(Return value): None
 *Description: makes a string path to the process folder
 */
int generateDirPath(char *taskPID, char *dirPath)
{
    sprintf(dirPath, "%s%s", PROC_PATH, taskPID);
    return 0;
}

/*
 *Function Name: processDirExists
 *Input to the method: *The path to the process directory
 *Output(Return value): None
 *Description: Parses the State field from the stat file
 */
int processDirExists(char *dirPath)
{
    DIR *dir;
    dir = opendir(dirPath);
    //try to open the directory
    //check if we had an error
    if (ENOENT == errno)
    {
        // The specified process directory does not exist
        printf("The specified process does not exist at %s\n", dirPath);
        closedir(dir);
        exit(ERROR_EXIT_STATUS);
    }
    closedir(dir);
    return 0;
}