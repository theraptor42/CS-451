//
// Created by caspi on 10/24/2018.
//

#include "Passenger.h"

void initPassengers()
{
    parsePassengerSchedules();

    //Initialize the passengers semaphore with a value of 1
    //I initialize it after the passengers are created because
    //I can guarantee that there will be no thread conflict before this point
    sem_init(&passengersSemaphore, SHARED_BETWEEN_THREADS, MUTEX_VALUE);
}


PASSENGER constructPassenger(int passengerId, char* inputLine)
{
    PASSENGER newPassenger;
    newPassenger.id = passengerId;
    newPassenger.currentStopIndex = 0;
    newPassenger.currentFloor = BOTTOM_FLOOR;

    char* leftOverBuffer = (char *)malloc(sizeof(char) * STDIN_BUFFER_SIZE);
    sscanf(inputLine, "%d %[^\n]", &newPassenger.totalStopCount, leftOverBuffer);
    // %[^\n] - accepts any char that is not a new line
    int counter;

    //allocate space for the schedule arrays
    newPassenger.wanderTimes = (int *)malloc(sizeof(int) * newPassenger.totalStopCount);
    newPassenger.destinationFloors = (int *)malloc(sizeof(int) * newPassenger.totalStopCount);

    for (counter = 0; counter < newPassenger.totalStopCount; counter++)
    {
        // Read an floor and a wander time and put the rest back in the buffer
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

void freePassenger(PASSENGER * passenger)
{
    free(passenger->destinationFloors);
    free(passenger->wanderTimes);
}

int getPassengerId(PASSENGER* passenger)
{
    sem_wait(&passengersSemaphore);
    int idValue = passenger->id;
    sem_post(&passengersSemaphore);
    return idValue;
}
int getPassengerTotalStopCount(PASSENGER* passenger)
{
    sem_wait(&passengersSemaphore);
    int stopCount = passenger->totalStopCount;
    sem_post(&passengersSemaphore);
    return stopCount;
}
unsigned int getPassengerCurrentFloor(PASSENGER *passenger)
{
    sem_wait(&passengersSemaphore);
    int currentFloor = passenger->currentFloor;
    sem_post(&passengersSemaphore);
    return (unsigned int) currentFloor;
}

unsigned int getPassengerNextFloor(PASSENGER *passenger)
{
    sem_wait(&passengersSemaphore);
    int nextFloor = passenger->destinationFloors[passenger->currentStopIndex ];
    sem_post(&passengersSemaphore);
    return (unsigned int) nextFloor;
}
unsigned int getPassengerCurrentWanderTime(PASSENGER *passenger)
{
    sem_wait(&passengersSemaphore);
    int wanderTime = passenger->wanderTimes[passenger->currentStopIndex];
    sem_post(&passengersSemaphore);
    return (unsigned int) wanderTime;
}

void incrementStopIndex(PASSENGER *passenger)
{
    sem_wait(&passengersSemaphore);
    passenger->currentStopIndex++;
    sem_post(&passengersSemaphore);
}

void setPasengerCurrentFloor(PASSENGER *passenger, int newCurrent)
{
    sem_wait(&passengersSemaphore);
    passenger->currentFloor = newCurrent;
    sem_post(&passengersSemaphore);
}

void signalElevator(PASSENGER* passenger)
{
    //atomic-ly get the required data fields
    int passengerId = getPassengerId(passenger);
    int currentFloor = getPassengerCurrentFloor(passenger);

    sem_wait(&printLock);
    fprintf(stdout, "Person %d: Waiting for elevator at floor %d\n", passengerId, currentFloor);
    fflush(stdout);
    sem_post(&printLock);

    sem_wait(&elevatorSemaphore);
    elevator.waitingForElevatorCount[currentFloor]++;
    sem_post(&elevatorSemaphore);

    //thread safe
    sem_wait(&sourceQueue[currentFloor]);
}

void boardElevator(PASSENGER* passenger)
{
    //wrap wait in condition for direction to get extra credit
    int passengerId = getPassengerId(passenger);
    int destinationFloor = getPassengerNextFloor(passenger);
    int currentFloor = getPassengerCurrentFloor(passenger);

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
        }

        sem_wait(&sourceQueue[currentFloor]);
    }

    sem_wait(&printLock);
    fprintf(stdout, "Person %d: Boarded elevator from floor %d to floor %d\n", passengerId, currentFloor, destinationFloor);
    fflush(stdout);
    sem_post(&printLock);

    sem_wait(&elevatorSemaphore);
    elevator.waitingForElevatorCount[currentFloor]--;
    elevator.internalQueueCount[destinationFloor]++;
    sem_post(&elevatorSemaphore);

    //thread safe
    sem_wait(&destinationQueue[destinationFloor]);
}

void wander(PASSENGER* passenger)
{
    int passengerId = getPassengerId(passenger);
    int destinationFloor = getPassengerNextFloor(passenger);
    int wanderTime = getPassengerCurrentWanderTime(passenger);
    //The index hasn't moved yet, but the passenger is at the new floor now
    sem_wait(&elevatorSemaphore);
    elevator.internalQueueCount[destinationFloor]--;
    sem_post(&elevatorSemaphore);

    sem_wait(&printLock);
    fprintf(stdout, "Passenger %d: Got off elevator at floor %d\n", passengerId, destinationFloor);
    fprintf(stdout, "Passenger %d: Wandering for %d second(s)\n", passengerId, wanderTime);
    fflush(stdout);
    sem_post(&printLock);

    sleep((unsigned int) wanderTime);
}


void *passengerThreadProcess(void *vargp)
{
    PASSENGER* threadPassenger = (PASSENGER*)vargp;
    int scheduleIndex;
    sem_wait(&passengersSemaphore);
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
        setPasengerCurrentFloor(threadPassenger, getPassengerNextFloor(threadPassenger));
        incrementStopIndex(threadPassenger);
    }
    //while(setPasengerCurrentFloor(threadPassenger));

    sem_wait(&printLock);
    fprintf(stdout, "Passenger %d: Left the building.  Thread exiting.\n", getPassengerId(threadPassenger));
    fflush(stdout);
    sem_post(&printLock);


    freePassenger(threadPassenger);
    pthread_exit(0);
}