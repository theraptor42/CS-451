//
// Created by caspi on 11/2/2018.
//

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
PASSENGER* passengers;//
ELEVATOR elevator;

Semaphore passengersSemaphore;
Semaphore elevatorSemaphore;
Semaphore optionsAccessSemaphore;
Semaphore printLock;

Semaphore* destinationQueue;
Semaphore* sourceQueue;



#endif //ASSIGNMENT_2_MAIN_H
