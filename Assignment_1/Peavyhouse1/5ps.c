/*
 *Author: Caspian Peavyhouse
 *Assignment Number: A1
 *Date of Submission: 10/17/2018
 *Name of this file: 5ps.c
 *Description of the program:
 * 5ps - A mini clone of the ps command
 *     - Provides basic info about a task
 */


#include <stdlib.h>
#include "OptionsProcessing.h"
#include "Parser.h"


/*
 *Function Name: main
 *Input to the method: Command Line Arguments
 *Output(Return value): None(Exit Status)
 *Description: Pulls the Parser and OptionsProcessing
 *             modules together to provide output.
 */
int main(int argc, char **argv)
{
    //Declare some variables used and allocate some space in memory for them
    char* pidPath;
    char* outputString;
    char* taskPid = (char *)malloc(sizeof(char) * PID_MAX_LENGTH);
    pidPath = (char *)malloc(sizeof(char) * (PROC_PATH_LENGTH + PID_MAX_LENGTH));

    //Get the list of options the user selected
    short optionMask = readOptions(argc, argv, pidPath, taskPid);

    //Get the data the user requested
    outputString = parseProcFiles(optionMask, taskPid, pidPath);

    //Print the output if there is any
    if (outputString != NULL)
    {
        printf("%s", outputString);
        free(outputString);
    }

    //Release the resources used
    free(pidPath);
    free(taskPid);
    return 0;
}

