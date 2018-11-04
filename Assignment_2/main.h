/*
 *Author: Caspian Peavyhouse
 *Assignment Number: A2
 *Date of Submission: 11/03/2018
 *Name of this file: main.h
 *Description of the program:
 * Elevator - Simulation of an elevator system
 */

#ifndef ASSIGNMENT_2_MAIN_H
#define ASSIGNMENT_2_MAIN_H

//Includes
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>  //Header file for sleep(). man 3 sleep for details.
#include <pthread.h>
#include <semaphore.h>
typedef sem_t       Semaphore; //I prefer to use a more symbolic name
typedef pthread_t   Thread;

#include "ParseOptions.h"
#include "Passenger.h"
#include "Elevator.h"



//Error exit status table
#define MAIN_ERROR_EXIT_STATUS          (10)
#define OPTIONS_ERROR_EXIT_STATUS       (20)
#define PASSENGER_ERROR_EXIT_STATUS     (30)

#define SHARED_BETWEEN_THREADS          0
#define MUTEX_VALUE                     1
#define INITIAL_QUEUE_LENGTH            0

#define TRUE                            1
#define FALSE                           0



//Global Variables
PASSENGER* passengers;//array that holds all the passenger data
ELEVATOR elevator;//the elevator struct

//protect the various shared data fields
Semaphore passengersSemaphore;
Semaphore elevatorSemaphore;
Semaphore optionsAccessSemaphore;
Semaphore printLock;

//the array of queues for each floor
Semaphore* destinationQueue;//inside the elevator
Semaphore* sourceQueue;//outside the elevator



#endif //ASSIGNMENT_2_MAIN_H
