//
// Created by caspi on 11/2/2018.
//

#ifndef ASSIGNMENT_2_MAIN_H
#define ASSIGNMENT_2_MAIN_H


//Includes
#include "ParseOptions.h"
#include "Passenger.h"
#include "Elevator.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>  //Header file for sleep(). man 3 sleep for details.
#include <pthread.h>


//Error exit status table
#define MAIN_ERROR_EXIT_STATUS      (-1)
#define OPTIONS_ERROR_EXIT_STATUS   (-2)

#define MAX_NUMBER_OF_PASSENGERS     200

//Global Variables
int numberOfPeople;
int maxWanderTime;
int numberOfFloors;

PASSENGER passengers [MAX_NUMBER_OF_PASSENGERS];



#endif //ASSIGNMENT_2_MAIN_H
