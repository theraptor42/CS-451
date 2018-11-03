//
// Created by caspi on 11/2/2018.
//

#include "main.h"

#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>

int numberOfPeople;
int maxWanderTime;
int numberOfFloors;

FILE *input;
char* parseOptions(int argc, char **argv)
{
    int getOptStatus;
    //Default values
    numberOfPeople = 1;
    maxWanderTime  = 10;
    numberOfFloors = 10;
    waitForCorrectDirection = FALSE;

    int fTest = 0;
    //read the option string
    while ((getOptStatus = getopt (argc, argv, "f:p:w:td")) != -1)
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
            case 't':
                fTest = 1;
                break;
            case 'd':
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

    if(!fTest){
        input = stdin;
    } else {
        input = fopen("input.txt","r");
    }

    if (numberOfFloors < 1 || numberOfPeople < 1 || maxWanderTime < 1)
    {
        printf("There was an error processing the input arguments.  ");
        printf("Please ensure that all parameters are positive integers\n");
        exit(OPTIONS_ERROR_EXIT_STATUS);
    }
    return NULL;
}

int parsePassengerSchedules()
{
    char buffer[STDIN_BUFFER_SIZE];
    char* result;
    int passengerCount = 0;
    while((fgets(buffer, STDIN_BUFFER_SIZE , input)) != NULL)
    {
        PASSENGER newPassenger = constructPassenger(passengerCount, buffer);
        passengers[newPassenger.id] = newPassenger;
        passengerCount++;
    }
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

int getNumberOfPeople()
{
    sem_wait(&optionsAccessSemaphore);
    int returnValue = numberOfPeople;
    sem_post(&optionsAccessSemaphore);
    return returnValue;
}
int getMaxWanderTime()
{
    sem_wait(&optionsAccessSemaphore);
    int returnValue = maxWanderTime;
    sem_post(&optionsAccessSemaphore);
    return returnValue;
}
int getNumberOfFloors()
{
    sem_wait(&optionsAccessSemaphore);
    int returnValue = numberOfFloors;
    sem_post(&optionsAccessSemaphore);
    return returnValue;
}