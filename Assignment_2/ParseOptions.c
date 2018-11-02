//
// Created by caspi on 11/2/2018.
//

#include "main.h"

#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>



char* parseOptions(int argc, char **argv)
{
    int getOptStatus;
    //
    numberOfPeople = 1;
    maxWanderTime  = 10;
    numberOfFloors = 10;
    //read the option string
    while ((getOptStatus = getopt (argc, argv, "f:p:w:")) != -1)
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

    return NULL;
}

int parsePassengerSchedules()
{
    char buffer[STDIN_BUFFER_SIZE];
    char* result;
    int passengerCount = 0;
    while((result = fgets(buffer, STDIN_BUFFER_SIZE , stdin)) != NULL)
    {
        PASSENGER newPassenger = constructPassenger(passengerCount, buffer);
        passengers[newPassenger.id] = newPassenger;
    }

    return 0;
}