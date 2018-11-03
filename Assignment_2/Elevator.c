//
// Created by caspi on 10/24/2018.
//

#include "Elevator.h"

void *elevatorThreadProcess(void *vargp)
{
    sleep(SLEEP_TIME);
    printPassengersWaitingToBoard();

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

            if (arePassengersWaitingInElevator(currentFloor))
                disembarkElevator();
            if (arePassengersWaitingAtFloor(currentFloor))
                loadElevator();
        }
        moveElevator();
    }
}

void initElevator()
{
    elevator.currentFloor = BOTTOM_FLOOR;
    elevator.direction = ELEVATOR_UP;
    elevator.waitingForElevatorCount = (int*)malloc(sizeof(int) * numberOfFloors);
    elevator.internalQueueCount = (int*)malloc(sizeof(int) * numberOfFloors);

    sem_init(&elevatorSemaphore, SHARED_BETWEEN_THREADS, MUTEX_VALUE);

    //Initialize the button press  and  internal elevator queues (semaphores)
    int counter;
    for (counter = 0; counter < numberOfFloors; counter++)
    {
        sem_init(&sourceQueue[counter], SHARED_BETWEEN_THREADS, INITIAL_QUEUE_LENGTH);
        sem_init(&destinationQueue[counter], SHARED_BETWEEN_THREADS, INITIAL_QUEUE_LENGTH);
    }

}

void freeElevator()
{
    free(elevator.waitingForElevatorCount);
    free(elevator.internalQueueCount);
}

void moveElevator()
{

    //Thread safe calls
    correctElevatorDirection();

    if (!arePassengersWaitingAtFloor(COUNT_ALL_FLOORS) && !arePassengersWaitingInElevator(COUNT_ALL_FLOORS)
        && getElevatorCurrentFloor() == BOTTOM_FLOOR)
    {
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

    if (startingDirection != newDirection)
    {
        sem_wait(&printLock);
        if (newDirection == ELEVATOR_UP)
            fprintf(stdout, "The elevator is now traveling up\n");
        else
            fprintf(stdout, "The elevator is now traveling down\n");
        fflush(stdout);
        sem_post(&printLock);

        if (newDirection == ELEVATOR_UP)
            printPassengersWaitingToBoard();
    }
}

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

void incrementElevatorFloor()
{
    sem_wait(&elevatorSemaphore);
    elevator.currentFloor = elevator.currentFloor + elevator.direction;//direction is either a +1 or a -1
    sem_post(&elevatorSemaphore);
}

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

        sem_post(&sourceQueue[floor]); //let a waiting task board the elevator
        sem_post(&elevatorSemaphore);
    }
}

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

int getElevatorDirection()
{
    sem_wait(&elevatorSemaphore);
    int returnValue = elevator.direction;
    sem_post(&elevatorSemaphore);
    return returnValue;
}

int getElevatorCurrentFloor()
{
    sem_wait(&elevatorSemaphore);
    int returnValue = elevator.currentFloor;
    sem_post(&elevatorSemaphore);
    return returnValue;
}

int countPassengersWaitingAtFloor(int floor)
{
    sem_wait(&elevatorSemaphore);
    int returnValue = elevator.waitingForElevatorCount[floor];
    sem_post(&elevatorSemaphore);
    return returnValue;
}

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

int countPassengersWaitingInElevator(int destFloor)
{
    sem_wait(&elevatorSemaphore);
    int returnValue = elevator.internalQueueCount[destFloor];
    sem_post(&elevatorSemaphore);
    return returnValue;
}

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


