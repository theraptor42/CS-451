//
// Created by caspi on 10/24/2018.
//

#ifndef ASSIGNMENT_2_ELEVATOR_H
#define ASSIGNMENT_2_ELEVATOR_H


//Typedefs
struct Elevator
{
    int direction;
    int currentFloor;
    int* waitingForElevatorCount;
    int* internalQueueCount;
};
typedef struct Elevator ELEVATOR;


//Includes
#include "main.h"
#include "Passenger.h"
#include <unistd.h>

//defines
#define SLEEP_TIME              1 //One second
#define BOTTOM_FLOOR            0
#define ELEVATOR_UP             1
#define ELEVATOR_DOWN           -1
#define COUNT_ALL_FLOORS        -1


//Function prototypes
void *elevatorThreadProcess(void *vargp);

void initElevator();
void freeElevator();

void moveElevator();
void correctElevatorDirection();
void printPassengersWaitingToBoard();
void incrementElevatorFloor();

void loadElevator();
void disembarkElevator();

int getElevatorDirection();;
int getElevatorCurrentFloor();

int countPassengersWaitingAtFloor(int floor);
int arePassengersWaitingAtFloor(int floor);

int countPassengersWaitingInElevator(int destFloor);
int arePassengersWaitingInElevator(int destFloor);


#endif //ASSIGNMENT_2_ELEVATOR_H
