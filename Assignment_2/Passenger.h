//
// Created by caspi on 10/24/2018.
//

#ifndef ASSIGNMENT_2_PASSENGER_H
#define ASSIGNMENT_2_PASSENGER_H

//I didn't know this, but apparently the order it appears in the file is important
//I had to move this before the #includes because everything else references the passenger struct
//And it couldn't recognize the type unless it was defined before the include
struct Passenger
{
    int id; //What index in the passenger data array will I find this
    int currentFloor; //Where am I now?
    int totalStopCount; //total number of stops
    int currentStopIndex;  //Index of the current stop in the array
    int* destinationFloors;// [MAX_NUMBER_OF_STOPS];
    int* wanderTimes;// [MAX_NUMBER_OF_STOPS];
};

//Keyword the PASSENGER struct
typedef struct Passenger PASSENGER;


#include "main.h"
#include "ParseOptions.h"
#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>


//Function prototypes
void initPassengers();
PASSENGER constructPassenger(int passengerId, char* inputLine);
void freePassenger(PASSENGER* passenger);
int getPassengerId(PASSENGER* passenger);
int getPassengerTotalStopCount(PASSENGER* passenger);
unsigned int getPassengerCurrentFloor(PASSENGER *passenger);
unsigned int getPassengerNextFloor(PASSENGER *passenger);
unsigned int getPassengerCurrentWanderTime(PASSENGER *passenger);
void incrementStopIndex(PASSENGER *passenger);
void setPasengerCurrentFloor(PASSENGER *passenger, int newCurrent);
void boardElevator(PASSENGER* passenger);
void signalElevator(PASSENGER* passenger);
void wander(PASSENGER* passenger);

void *passengerThreadProcess(void *vargp);

#endif //ASSIGNMENT_2_PASSENGER_H
