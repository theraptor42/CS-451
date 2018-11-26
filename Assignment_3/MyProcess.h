//
// Created by caspi on 11/25/2018.
//

#ifndef ASSIGNMENT_3_MYPROCESS_H
#define ASSIGNMENT_3_MYPROCESS_H

//Includes
#include <sys/time.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>


//Struct definition
struct process
{
    int processNumber;
    int arrivalTime;
    int burstTime;
    int priority;
    __pid_t pid;
};
typedef struct process PROCESS;


//Defines

//Global Variables

//Function Prototypes
void constructProcess(char* inputLine, PROCESS *newProcess);
__pid_t initializeProcess(PROCESS *process, int currentTime);
int resumeProcess(PROCESS *process, int currentTime);
int suspendProcess(PROCESS *process, int currentTime);
int terminateProcess(PROCESS *process, int currentTime);
int burstProcessOneSecond(PROCESS* process);






#endif //ASSIGNMENT_3_MYPROCESS_H
