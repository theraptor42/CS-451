//
// Created by caspi on 11/15/2018.
//

#ifndef ASSIGNMENT_3_SCHEDULER_H
#define ASSIGNMENT_3_SCHEDULER_H


//Include needed for the struct
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <signal.h>
#include <string.h>
#include "MyProcess.h"
#include "ProcessLinkedList.h"


//Struct definition
struct processScheduler
{
    int numberOfProcesses;
    int processesCompleted;
    PROCESS processArray [10];
    ProcessLinkedList priorityQueue;
    struct itimerval timer;
    int currentTime;
};

typedef struct processScheduler SCHEDULER;

//defines
#define OPTIONS_ERROR_EXIT_STATUS       2
#define STDIN_BUFFER_SIZE               100


//Global variables
SCHEDULER scheduler;
//struct itimerval timer;

//Function prototypes
int schedulerProgram();
int parseSchedule();
void initTimer();
void timerHandler(int signum);


#endif //ASSIGNMENT_3_SCHEDULER_H
