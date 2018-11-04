/*
 *Author: Caspian Peavyhouse
 *Assignment Number: A2
 *Date of Submission: 11/03/2018
 *Name of this file: ParseOptions.c
 *Description of the program:
 * Elevator - Simulation of an elevator system
 */

#include "main.h"

#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>

//this is here because I was having some weird errors
int numberOfPeople;
int maxWanderTime;
int numberOfFloors;

//used for executing from windows
FILE *input; // source of the passenger schedules


/*
 *Function Name: parseOptions
 *Input to the method: the parameters from the command line
 *Output(Return value): void
 *Description: check the parameters provided by the user
 */
void parseOptions(int argc, char **argv)
{
    int getOptStatus;
    //Default values
    numberOfPeople = 1;
    maxWanderTime  = 10;
    numberOfFloors = 10;
    waitForCorrectDirection = FALSE;

    int fileTest = 0;// windows os check
    //read the option string
    while ((getOptStatus = getopt (argc, argv, "tdf:p:w:")) != -1)
    {
        switch (getOptStatus)
        {
            //Sscanf - quick and dirty str -> int conversion
            case 'f':
                sscanf(optarg, "%d", &numberOfFloors);
                break;
            case 'p':
                sscanf(optarg, "%d", &numberOfPeople);
                break;
            case 'w':
                sscanf(optarg, "%d", &maxWanderTime);
                break;
            //case 't':
                //windows can't handle the < so we have to give it a file to fopen
                //fileTest = 1; // feature removed from final product
                //break;
            case 'd':
                //Extra Credit - d for direction
                //if true, passengers will wait for correct direction
                waitForCorrectDirection = TRUE;
                break;

                //This is what it does when it doesn't recognize a character
            case '?':
                if (optopt == 'f' || optopt == 'p' || optopt == 'w')
                {
                    //The flag was set but no argument was provided
                    printf("You must provide an int argument with the -%c option.\n", optopt);
                    exit(OPTIONS_ERROR_EXIT_STATUS);
                }
                else
                {
                    printf("Unknown option `-%c'.\n", optopt);
                    exit(OPTIONS_ERROR_EXIT_STATUS);
                }

            default:
                exit(OPTIONS_ERROR_EXIT_STATUS);
        }
    }

    if(!fileTest)
    {
        input = stdin;
    } else
    {
        //hard coded value for my local machine
        input = fopen("input.txt","r");
    }

    if (numberOfFloors < 1 || numberOfPeople < 1 || maxWanderTime < 1)
    {
        printf("There was an error processing the input arguments.  ");
        printf("Please ensure that all parameters are positive integers\n");
        exit(OPTIONS_ERROR_EXIT_STATUS);
    }
}


/*
 *Function Name: parsePassengerSchedules
 *Input to the method: none
 *Output(Return value): int - successful execution
 *Description: parse the input file of passenger schedules
 */
int parsePassengerSchedules()
{
    char buffer[STDIN_BUFFER_SIZE];
    char* result;
    int passengerCount = 0;
    //build the passengers array
    while((result = fgets(buffer, STDIN_BUFFER_SIZE , input)) != NULL && result != '\0')
    {
        PASSENGER newPassenger = constructPassenger(passengerCount, buffer);
        passengers[newPassenger.id] = newPassenger;
        passengerCount++;
    }
    //check if all the numbers add up
    if (passengerCount > numberOfPeople)
    {
        printf("Too many schedules were provided.  %d passengers were specified but %d schedules were given.\n",
               numberOfPeople, passengerCount);
        exit(OPTIONS_ERROR_EXIT_STATUS);
    }
    else if (passengerCount < numberOfPeople)
    {
        printf("Too few schedules were provided.  %d passengers were specified but %d schedules were given.\n",
               numberOfPeople, passengerCount);
        exit(OPTIONS_ERROR_EXIT_STATUS);
    }
    return 0;
}

/*
 *Function Name: getMaxWanderTime
 *Input to the method: none
 *Output(Return value): int - the maximum wander time
 *Description: thread safe access to the maximum wander time
 */
int getMaxWanderTime()
{
    sem_wait(&optionsAccessSemaphore);
    int returnValue = maxWanderTime;
    sem_post(&optionsAccessSemaphore);
    return returnValue;
}

/*
 *Function Name: getNumberOfFloors
 *Input to the method: none
 *Output(Return value): int - the number of floors
 *Description: thread safe access to the number of floor
 */
int getNumberOfFloors()
{
    sem_wait(&optionsAccessSemaphore);
    int returnValue = numberOfFloors;
    sem_post(&optionsAccessSemaphore);
    return returnValue;
}