/*
 *Author: Caspian Peavyhouse
 *Assignment Number: A2
 *Date of Submission: 11/03/2018
 *Name of this file: ParseOptions.h
 *Description of the program:
 * Elevator - Simulation of an elevator system
 */

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
void parseOptions(int argc, char **argv);
int parsePassengerSchedules();

int getNumberOfPeople();
int getMaxWanderTime();
int getNumberOfFloors();

#endif //ASSIGNMENT_2_PARSEOPTIONS_H
