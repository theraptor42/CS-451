//
// Created by caspi on 10/24/2018.
//

#include "Passenger.h"


PASSENGER constructPassenger(int passengerId, char* inputLine)
{
    PASSENGER newPassenger;
    newPassenger.id = passengerId;
    newPassenger.currentStopIndex = 0;

    char* leftOverBuffer = (char *)malloc(sizeof(char) * STDIN_BUFFER_SIZE);
    sscanf(inputLine, "%d %[^\n]", &newPassenger.totalStopCount, leftOverBuffer);
    // %[^\n] - accepts any char that is not a new line
    int counter;

    for (counter = 0; counter < newPassenger.totalStopCount; counter++)
    {
        sscanf(leftOverBuffer, "%d %d %[^\n]", &newPassenger.destinationFloors[counter],
                &newPassenger.departureTimes[counter], leftOverBuffer);
    }

    free (leftOverBuffer);
    printf("New Passenger: %d\n", newPassenger.id);
    printf("Number of Stops: %d\n", newPassenger.totalStopCount);
    printf("First Stop: %d\n", newPassenger.destinationFloors[0]);
    printf("Idle Time: %d\n", newPassenger.departureTimes[0]);
    return newPassenger;
}
