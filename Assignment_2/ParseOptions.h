//
// Created by caspi on 11/2/2018.
//

#ifndef ASSIGNMENT_2_PARSEOPTIONS_H
#define ASSIGNMENT_2_PARSEOPTIONS_H

//Defines
#define STDIN_BUFFER_SIZE      100

//Global Variables
extern int numberOfPeople;
extern int maxWanderTime;
extern int numberOfFloors;
int waitForCorrectDirection;

//Function Headers
char* parseOptions(int argc, char **argv);
int parsePassengerSchedules();

int getNumberOfPeople();
int getMaxWanderTime();
int getNumberOfFloors();

#endif //ASSIGNMENT_2_PARSEOPTIONS_H
