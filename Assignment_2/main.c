//All my includes and defines are in my main.h header
#include "main.h"


int main(int argc, char **argv)
{
    //Read the passed in parameters and store the data in global variables
    parseOptions(argc, argv);

    //Allocate the space required for the passengers and the queues used
    passengers = (PASSENGER*)malloc(sizeof(PASSENGER) * numberOfPeople);
    sourceQueue = (Semaphore*)malloc(sizeof(Semaphore) * numberOfFloors);
    destinationQueue = (Semaphore*)malloc(sizeof(Semaphore) * numberOfFloors);

    //Protect access to the global options
    sem_init(&optionsAccessSemaphore, SHARED_BETWEEN_THREADS, MUTEX_VALUE);


    //Initialize the passengers from the input file
    initPassengers();

    //Initialize the elevator struct with default values
    // and initialize the semaphores used for the waiting queues
    initElevator();

    //Init the print semaphore
    sem_init(&printLock, SHARED_BETWEEN_THREADS, MUTEX_VALUE);

    //Create the tasks for the passengers
    Thread* passengerThreads[numberOfPeople];
    int counter;
    for (counter = 0; counter < numberOfPeople; counter++)
    {
        Thread newThreadId;
        pthread_create(&newThreadId, NULL, passengerThreadProcess, &passengers[counter]);

        passengerThreads[counter] = &newThreadId;
    }

    //create the tasks for the elevator
    Thread elevatorThread;
    pthread_create(&elevatorThread, NULL, elevatorThreadProcess, NULL);
    //join the elevator task.  It holds the conditions for the program exiting
    pthread_join(elevatorThread, NULL);


    // free the system resources
    free(sourceQueue);
    free(destinationQueue);
    free(passengers);
    freeElevator();
    //*/
    exit(0);
}

