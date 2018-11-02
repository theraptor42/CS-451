//
// Created by caspi on 10/24/2018.
//

#ifndef ASSIGNMENT_2_PASSENGER_H
#define ASSIGNMENT_2_PASSENGER_H

#include "ParseOptions.h"
#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>

#define MAX_NUMBER_OF_STOPS     200

struct Passenger
{
    int id; //What index in the passenger data array will I find this
    int totalStopCount;
    int currentStopIndex;  //Index of the current stop in the array
    int destinationFloors [MAX_NUMBER_OF_STOPS];
    int departureTimes [MAX_NUMBER_OF_STOPS];
};

//Keyword the PASSENGER struct
typedef struct Passenger PASSENGER;

PASSENGER constructPassenger(int passengerId, char* inputLine);

#endif //ASSIGNMENT_2_PASSENGER_H
