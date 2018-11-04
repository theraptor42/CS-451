/*
 *Author: Caspian Peavyhouse
 *Assignment Number: A2
 *Date of Submission: 11/03/2018
 *Name of this file: Elevator.c
 *Description of the program:
 * Elevator - Simulation of an elevator system
 */

#include "Elevator.h"


/*
 *Function Name: elevatorThreadProcess
 *Input to the method: void* - not used
 *Output(Return value): void
 *Description: thread process that simulates the elevator moving
 */
void *elevatorThreadProcess(void *vargp)
{
    sleep(SLEEP_TIME);
    printPassengersWaitingToBoard();

    //it will execute forever unitl the exit condition is reached, but that exit happens in the program
    while(1)
    {
        int currentFloor = getElevatorCurrentFloor();
        //Check source and destination semaphores
        if (arePassengersWaitingAtFloor(currentFloor) || arePassengersWaitingInElevator(currentFloor))
        {
            sem_wait(&printLock);
            fprintf(stdout, "The elevator is stopping at floor %d\n", currentFloor);
            fprintf(stdout, "Thread sleeping for %d seconds\n", SLEEP_TIME);
            fflush(stdout);
            sem_post(&printLock);
            sleep(SLEEP_TIME);

            //self explanatory method names
            if (arePassengersWaitingInElevator(currentFloor))
                disembarkElevator();
            if (arePassengersWaitingAtFloor(currentFloor))
                loadElevator();
        }
        //go the the next floor
        moveElevator();
    }
}

/*
 *Function Name: initElevator
 *Input to the method: none
 *Output(Return value): none (output is in global variable)
 *Description: initialize the elevator struct
 *             it's in a global variable
 */
void initElevator()
{
    //default values
    elevator.currentFloor = BOTTOM_FLOOR;
    elevator.direction = ELEVATOR_UP;
    //allocate queue space
    elevator.waitingForElevatorCount = (int*)malloc(sizeof(int) * numberOfFloors);
    elevator.internalQueueCount = (int*)malloc(sizeof(int) * numberOfFloors);

    // init the elvator protection semaphore
    sem_init(&elevatorSemaphore, SHARED_BETWEEN_THREADS, MUTEX_VALUE);

    //Initialize the button press  and  internal elevator queues (semaphores)
    int counter;
    for (counter = 0; counter < numberOfFloors; counter++)
    {
        sem_init(&sourceQueue[counter], SHARED_BETWEEN_THREADS, INITIAL_QUEUE_LENGTH);
        sem_init(&destinationQueue[counter], SHARED_BETWEEN_THREADS, INITIAL_QUEUE_LENGTH);
    }

}

/*
 *Function Name: freeElevator
 *Input to the method: none
 *Output(Return value): void
 *Description: free the space allocated by the elevator
 */
void freeElevator()
{
    free(elevator.waitingForElevatorCount);
    free(elevator.internalQueueCount);
}


/*
 *Function Name: moveElevator
 *Input to the method: none
 *Output(Return value): void
 *Description: thread safe move the elevator to the next floor
 *              handles direction change
 */
void moveElevator()
{

    //Thread safe calls
    //change the direction value if we need to
    correctElevatorDirection();

    //check if the elevator should sleep
    if (!arePassengersWaitingAtFloor(COUNT_ALL_FLOORS) && !arePassengersWaitingInElevator(COUNT_ALL_FLOORS)
        && getElevatorCurrentFloor() == BOTTOM_FLOOR)
    {
        //wait for max wander time
        int waitTime = getMaxWanderTime();
        sem_wait(&printLock);
        fprintf(stdout, "The elevator has no passengers waiting\n");
        fprintf(stdout, "Thread sleeping for %d seconds\n", waitTime);
        fflush(stdout);
        sem_post(&printLock);

        sleep((unsigned int) waitTime);
        if (!arePassengersWaitingAtFloor(COUNT_ALL_FLOORS))
        {
            sem_wait(&printLock);
            fprintf(stdout, "All passengers have left the building\n");
            fprintf(stdout, "Elevator Thread exiting\n");
            fflush(stdout);
            sem_post(&printLock);

            pthread_exit(0); //Exit conditions were met
            //The last person left the building
        }
    }

    //move to the next floor
    incrementElevatorFloor();
    sem_wait(&printLock);
    fprintf(stdout, "The elevator is moving to floor %d\n", getElevatorCurrentFloor());
    fprintf(stdout, "Thread sleeping for %d seconds\n", SLEEP_TIME);
    fflush(stdout);
    sem_post(&printLock);

    //Transit time to next floor
    sleep(SLEEP_TIME);
}

/*
 *Function Name: correctElevatorDirection
 *Input to the method: none
 *Output(Return value): void
 *Description: change elevator direction if it is at top or bottom floor
 */
void correctElevatorDirection()
{
    sem_wait(&elevatorSemaphore);
    int startingDirection = elevator.direction;
    if (elevator.direction + elevator.currentFloor >= numberOfFloors)
        elevator.direction = ELEVATOR_DOWN;
    else if (elevator.direction + elevator.currentFloor < BOTTOM_FLOOR)
        elevator.direction = ELEVATOR_UP;
    int newDirection = elevator.direction;
    sem_post(&elevatorSemaphore);

    //output if we changed direction
    if (startingDirection != newDirection)
    {
        sem_wait(&printLock);
        if (newDirection == ELEVATOR_UP)
            fprintf(stdout, "The elevator is now traveling up\n");
        else
            fprintf(stdout, "The elevator is now traveling down\n");
        fflush(stdout);
        sem_post(&printLock);

        //print the waiting queue if its the bottom floor
        if (newDirection == ELEVATOR_UP)
            printPassengersWaitingToBoard();
    }
}

/*
 *Function Name: printPassengersWaitingToBoard
 *Input to the method: none
 *Output(Return value): void
 *Description: thread safe print the waiting queue of passengers
 */
void printPassengersWaitingToBoard()
{
    int floors = getNumberOfFloors();
    int counter;
    sem_wait(&printLock);
    //Build the pretty output
    fprintf(stdout, "Number of people waiting at floor:\n");
    fprintf(stdout, "%s\t\t\t\t%s\n", "Floor Number", "Number of People");
    for (counter = 0; counter < floors; counter++)
    {
        fprintf(stdout, "%d\t\t\t\t%d\n", counter , countPassengersWaitingAtFloor(counter));
    }
    fflush(stdout);
    sem_post(&printLock);
}

/*
 *Function Name: incrementElevatorFloor
 *Input to the method: none
 *Output(Return value): void
 *Description: atomic increment the elevator floor
 */
void incrementElevatorFloor()
{
    sem_wait(&elevatorSemaphore);
    elevator.currentFloor = elevator.currentFloor + elevator.direction;//direction is either a +1 or a -1
    sem_post(&elevatorSemaphore);
}


/*
 *Function Name: loadElevator
 *Input to the method: none
 *Output(Return value): void
 *Description: signal the passengers waiting to enter the elevator
 */
void loadElevator()
{
    //atomic-ly get the required data fields
    int floor = getElevatorCurrentFloor();
    int peopleWaiting = countPassengersWaitingAtFloor(floor);

    //print elevator activity
    sem_wait(&printLock);
    fprintf(stdout, "The elevator is loading passengers from floor %d\n", floor);
    fflush(stdout);
    sem_post(&printLock);

    //signal the waiting passenger tasks
    int counter;
    for (counter = 0; counter < peopleWaiting; counter++)
    {
        sem_wait(&elevatorSemaphore); //protect access to the elevator semaphore queue
                                        //prevent weird timing issues

        sem_post(&sourceQueue[floor]); //let a waiting task board the elevator
        sem_post(&elevatorSemaphore);
    }
}

/*
 *Function Name: disembarkElevator
 *Input to the method: none
 *Output(Return value): void
 *Description: signal waiting passengers to leave the elevator
 */
void disembarkElevator()
{
    //atomic-ly get the required data fields
    int floor = getElevatorCurrentFloor();
    int peopleWaiting = countPassengersWaitingInElevator(floor);

    //print elevator activity
    sem_wait(&printLock);
    fprintf(stdout, "The elevator is delivering passengers to floor %d\n", floor);
    fflush(stdout);
    sem_post(&printLock);

    //signal the waiting passenger tasks
    int counter;
    for (counter = 0; counter < peopleWaiting; counter++)
    {
        sem_wait(&elevatorSemaphore); //protect access to the elevator semaphore queue

        sem_post(&destinationQueue[floor]); //let a waiting task leave the elevator
        sem_post(&elevatorSemaphore);
    }
}

/*
 *Function Name: getElevatorDirection
 *Input to the method: none
 *Output(Return value): int - matches either ELEVATOR_UP or ELEVATOR_DOWN
 *Description: get the elevator direction
 */
int getElevatorDirection()
{
    sem_wait(&elevatorSemaphore);
    int returnValue = elevator.direction;
    sem_post(&elevatorSemaphore);
    return returnValue;
}

/*
 *Function Name: getElevatorCurrentFloor
 *Input to the method: none
 *Output(Return value): int - matches either ELEVATOR_UP or ELEVATOR_DOWN
 *Description: get the elevator's current floor
 */
int getElevatorCurrentFloor()
{
    sem_wait(&elevatorSemaphore);
    int returnValue = elevator.currentFloor;
    sem_post(&elevatorSemaphore);
    return returnValue;
}

/*
 *Function Name: countPassengersWaitingAtFloor
 *Input to the method: int floor number to check
 *Output(Return value): int - number of passengers waiting
 *Description: count the passengers in the the queue
 */
int countPassengersWaitingAtFloor(int floor)
{
    sem_wait(&elevatorSemaphore);
    int returnValue = elevator.waitingForElevatorCount[floor];
    sem_post(&elevatorSemaphore);
    return returnValue;
}

/*
 *Function Name: arePassengersWaitingAtFloor
 *Input to the method: int - floor number to check
 *Output(Return value): int - either 1 or 0
 *Description: checks if passengers are waiting at a floor
 *              int -1 is provided, it checks every floor
 */
int arePassengersWaitingAtFloor(int floor)
{
    //Count all the passengers waiting to board
    if (floor == COUNT_ALL_FLOORS)
    {
        int floors = getNumberOfFloors();
        int counter;
        for (counter = 0; counter < floors; counter++)
        {
            if (countPassengersWaitingAtFloor(counter) > 0)
                return TRUE;
        }
        return FALSE;
    }
        //only get the passengers waiting at this floor
    else
    {
        return countPassengersWaitingAtFloor(floor) > 0;
    }
}

/*
 *Function Name: countPassengersWaitingInElevator
 *Input to the method: int - floor to check
 *Output(Return value): int - number of passengers waiting
 *Description: count the passengers in the the elevator queue
 */
int countPassengersWaitingInElevator(int destFloor)
{
    sem_wait(&elevatorSemaphore);
    int returnValue = elevator.internalQueueCount[destFloor];
    sem_post(&elevatorSemaphore);
    return returnValue;
}

/*
 *Function Name: arePassengersWaitingInElevator
 *Input to the method: int - the floor to check for
 *Output(Return value): int - either 1 or 0
 *Description: checks if passengers are waiting for a floor in the elevator
 *              int -1 is provided, it checks for every floor
 */
int arePassengersWaitingInElevator(int destFloor)
{
    //see if anyone is waiting in the elevator
    if (destFloor == COUNT_ALL_FLOORS)
    {
        int floors = getNumberOfFloors();
        int counter;
        for (counter = 0; counter < floors; counter++)
        {
            if (countPassengersWaitingInElevator(counter) > 0)
                return TRUE;
        }
        return FALSE;
    }
    // see if anyone is waiting in the elevator for this floor
    else
    {
        return countPassengersWaitingInElevator(destFloor) > 0;
    }
}


