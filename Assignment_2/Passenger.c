/*
 *Author: Caspian Peavyhouse
 *Assignment Number: A2
 *Date of Submission: 11/03/2018
 *Name of this file: Passenger.c
 *Description of the program:
 * Elevator - Simulation of an elevator system
 */

#include "Passenger.h"

/*
 *Function Name: InitPassengers
 *Input to the method: None, triggers other programs
 *Output(Return value): None
 *Description: Triggers parsing the passenger schedules
 *             Initializes the passengers semaphore
 */
void initPassengers()
{
    parsePassengerSchedules();

    //Initialize the passengers semaphore with a value of 1
    //I initialize it after the passengers are created because
    //I can guarantee that there will be no thread conflict before this point
    sem_init(&passengersSemaphore, SHARED_BETWEEN_THREADS, MUTEX_VALUE);
}

/*
 *Function Name: constructPassenger
 *Input to the method: int passenger id - id of the new passenger
 *                     char* input line - the line we're parsing for the schedule
 *Output(Return value): PASSENGER - the passeger constructed
 *Description: creates and populates a passenger struct
 *             from a line of the schedule file
 */
PASSENGER constructPassenger(int passengerId, char* inputLine)
{
    //Create a new passenger
    PASSENGER newPassenger;
    newPassenger.id = passengerId;
    newPassenger.currentStopIndex = 0;
    newPassenger.currentFloor = BOTTOM_FLOOR;

    //Read the first char into totalStopCount
    char* leftOverBuffer = (char *)malloc(sizeof(char) * STDIN_BUFFER_SIZE);
    sscanf(inputLine, "%d %[^\n]", &newPassenger.totalStopCount, leftOverBuffer);
    // %[^\n] - accepts any char that is not a new line
    int counter;

    //allocate space for the schedule arrays
    newPassenger.wanderTimes = (int *)malloc(sizeof(int) * newPassenger.totalStopCount);
    newPassenger.destinationFloors = (int *)malloc(sizeof(int) * newPassenger.totalStopCount);

    for (counter = 0; counter < newPassenger.totalStopCount; counter++)
    {
        // Read a floor and a wander time and put the rest back in the buffer
        sscanf(leftOverBuffer, "%d %d %[^\n]", &newPassenger.destinationFloors[counter],
                &newPassenger.wanderTimes[counter], leftOverBuffer);

        //Check if the read floor and wander time are valid
        if (newPassenger.destinationFloors[counter] >= numberOfFloors)
        {
            printf("The destination floor %d for passenger %d is higher than the %d floors in the building.\n",
                    newPassenger.destinationFloors[counter], newPassenger.id, numberOfFloors);

            free(leftOverBuffer);
            exit(PASSENGER_ERROR_EXIT_STATUS);
        }
        if (newPassenger.wanderTimes[counter] > maxWanderTime)
        {
            printf("The wander time %d for passenger %d is longer than the %d max wander time specified.\n",
                   newPassenger.wanderTimes[counter], newPassenger.id, maxWanderTime);

            free (leftOverBuffer);
            exit(PASSENGER_ERROR_EXIT_STATUS);
        }

    }
    free (leftOverBuffer);
    return newPassenger;
}

/*
 *Function Name: freePassenger
 *Input to the method: Passenger* - pointer to the passenger struct
 *Output(Return value): None
 *Description: free the resources in a passenger struct after use
 */
void freePassenger(PASSENGER * passenger)
{
    free(passenger->destinationFloors);
    free(passenger->wanderTimes);
}

/*
 *Function Name: getPassengerId
 *Input to the method: Passenger* - pointer to the passenger struct
 *Output(Return value): int - the id of the passed passsenger
 *Description: thread safe get for the data field
 */
int getPassengerId(PASSENGER* passenger)
{
    sem_wait(&passengersSemaphore);
    int idValue = passenger->id;
    sem_post(&passengersSemaphore);
    return idValue;
}

/*
 *Function Name: getPassengerCurrentFloor
 *Input to the method: Passenger* - pointer to the passenger struct
 *Output(Return value): unsigned int - the floor the passenger is currently on
 *Description: thread safe get for the data field
 */
unsigned int getPassengerCurrentFloor(PASSENGER *passenger)
{
    sem_wait(&passengersSemaphore);
    int currentFloor = passenger->currentFloor;
    sem_post(&passengersSemaphore);
    return (unsigned int) currentFloor;
}

/*
 *Function Name: getPassengerNextFloor
 *Input to the method: Passenger* - pointer to the passenger struct
 *Output(Return value): unsigned int - the floor number of the destination floor
 *Description: thread safe get for the data field
 */
unsigned int getPassengerNextFloor(PASSENGER *passenger)
{
    sem_wait(&passengersSemaphore);
    int nextFloor = passenger->destinationFloors[passenger->currentStopIndex ];
    sem_post(&passengersSemaphore);
    return (unsigned int) nextFloor;
}

/*
 *Function Name: getPassengerCurrentWanderTime
 *Input to the method: Passenger* - pointer to the passenger struct
 *Output(Return value): unsigned int - how long the passenger after it arrives at the destination floor
 *Description: thread safe get for the data field
 */
unsigned int getPassengerCurrentWanderTime(PASSENGER *passenger)
{
    sem_wait(&passengersSemaphore);
    int wanderTime = passenger->wanderTimes[passenger->currentStopIndex];
    sem_post(&passengersSemaphore);
    return (unsigned int) wanderTime;
}

/*
 *Function Name: incrementStopIndex
 *Input to the method: Passenger* - pointer to the passenger struct
 *Output(Return value): void
 *Description: thread safe increment the stop index in the elevator struct
 *              This will cause the next floor() and currentwandertime() to change
 */
void incrementStopIndex(PASSENGER *passenger)
{
    sem_wait(&passengersSemaphore);
    passenger->currentStopIndex++;
    sem_post(&passengersSemaphore);
}

/*
 *Function Name: setPassengerCurrrentFloor
 *Input to the method: Passenger* - pointer to the passenger struct
 *                     int - the new currentFloor
 *Output(Return value): void
 *Description: 'move' the pasenger to the new current floor (thread safe)
 */
void setPasengerCurrentFloor(PASSENGER *passenger, int newCurrent)
{
    sem_wait(&passengersSemaphore);
    passenger->currentFloor = newCurrent;
    sem_post(&passengersSemaphore);
}


/*
 *Function Name: signalElevator
 *Input to the method: Passenger* - pointer to the passenger struct
 *Output(Return value): void
 *Description: thread safe signal the elevator to let it know you want it to stop
 */
void signalElevator(PASSENGER* passenger)
{
    //atomic-ly get the required data fields
    int passengerId = getPassengerId(passenger);
    int currentFloor = getPassengerCurrentFloor(passenger);

    sem_wait(&printLock);
    fprintf(stdout, "Person %d: Waiting for elevator at floor %d\n", passengerId, currentFloor);
    fflush(stdout);
    sem_post(&printLock);

    //increment the waiting count
    sem_wait(&elevatorSemaphore);
    elevator.waitingForElevatorCount[currentFloor]++;
    sem_post(&elevatorSemaphore);

    //thread safe actually wait for the elevtor to stop
    sem_wait(&sourceQueue[currentFloor]);
}

/*
 *Function Name: boardElevator
 *Input to the method: Passenger* - pointer to the passenger struct
 *Output(Return value): void
 *Description: move from the source queue to the destination queue
 *              waiting inside the elevator
 */
void boardElevator(PASSENGER* passenger)
{
    //Get these all atomic-ly so I don't have to worry about deadlock later
    int passengerId = getPassengerId(passenger);
    int destinationFloor = getPassengerNextFloor(passenger);
    int currentFloor = getPassengerCurrentFloor(passenger);


    /*
     * Extra credit section.  If the -d flag (for direction) is set
     * waitForCorrectDirectionwill be on.
     * The passenger checks if the elevator is going the right direction when it is woken.
     * If it is the wrong direction, it re-enters the source quere for the next cycle
     */
    if (waitForCorrectDirection)
    {
        int elevatorDirection = getElevatorDirection();
        if (((destinationFloor < currentFloor) && elevatorDirection == ELEVATOR_UP)  ||
                ((destinationFloor > currentFloor) && elevatorDirection == ELEVATOR_DOWN))
        {
            sem_wait(&printLock);
            fprintf(stdout, "Person %d: Waiting for the elevator to travel right direction\n", passengerId);
            fflush(stdout);
            sem_post(&printLock);

            sem_wait(&sourceQueue[currentFloor]);
        }

    }
    //End EC portion

    //protect the print statements
    sem_wait(&printLock);
    fprintf(stdout, "Person %d: Boarded elevator from floor %d to floor %d\n", passengerId, currentFloor, destinationFloor);
    fflush(stdout);
    sem_post(&printLock);

    //change queues
    sem_wait(&elevatorSemaphore);
    elevator.waitingForElevatorCount[currentFloor]--;
    elevator.internalQueueCount[destinationFloor]++;
    sem_post(&elevatorSemaphore);

    //thread safe wait inside the elevator
    sem_wait(&destinationQueue[destinationFloor]);
}

/*
 *Function Name: incrementStopIndex
 *Input to the method: Passenger* - pointer to the passenger struct
 *Output(Return value): void
 *Description: leave the elevator and wander for the given time
 */
void wander(PASSENGER* passenger)
{
    int passengerId = getPassengerId(passenger);
    int destinationFloor = getPassengerNextFloor(passenger);
    int wanderTime = getPassengerCurrentWanderTime(passenger);

    //leave the elevator
    sem_wait(&elevatorSemaphore);
    elevator.internalQueueCount[destinationFloor]--;
    sem_post(&elevatorSemaphore);

    sem_wait(&printLock);
    fprintf(stdout, "Passenger %d: Got off elevator at floor %d\n", passengerId, destinationFloor);
    fprintf(stdout, "Passenger %d: Wandering for %d second(s)\n", passengerId, wanderTime);
    fflush(stdout);
    sem_post(&printLock);

    //sleep for the wander time
    sleep((unsigned int) wanderTime);
}


/*
 *Function Name: passengerThreadProcess
 *Input to the method: void* - pointer to the passenger struct
 *Output(Return value): void
 *Description: the process executed by the thread.  simulates a passenger
 */
void *passengerThreadProcess(void *vargp)
{
    PASSENGER* threadPassenger = (PASSENGER*)vargp;
    int scheduleIndex;
    sem_wait(&passengersSemaphore);
    //how many loops am I going to run?
    int scheduledStops = threadPassenger->totalStopCount;
    sem_post(&passengersSemaphore);

    for (scheduleIndex = 0; scheduleIndex < scheduledStops; scheduleIndex++)
    {
        //call the elevator
        signalElevator(threadPassenger);
        //transfer to internal queue
        boardElevator(threadPassenger);
        //leave elevator queue
        //wait for wander time
        wander(threadPassenger);
        //move the previous next floor to current floor
        setPasengerCurrentFloor(threadPassenger, getPassengerNextFloor(threadPassenger));
        incrementStopIndex(threadPassenger);
    }
    //while(setPasengerCurrentFloor(threadPassenger));

    sem_wait(&printLock);
    fprintf(stdout, "Passenger %d: Left the building.  Thread exiting.\n", getPassengerId(threadPassenger));
    fflush(stdout);
    sem_post(&printLock);

    //free the passenger resources after execution
    freePassenger(threadPassenger);
    pthread_exit(0);
}