//
// Created by caspi on 11/15/2018.
//

#ifndef ASSIGNMENT_3_SCHEDULER_H
#define ASSIGNMENT_3_SCHEDULER_H

//Includes
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <signal.h>
#include <sys/time.h>
#include <string.h>


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

struct processScheduler
{
    int numberOfProcesses;
    PROCESS processArray [10];
    struct itimerval timer;
    int runningProcess;
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
PROCESS constructProcess(char* inputLine);
__pid_t initializeProcess(PROCESS *process);
void initTimer();
void timerHandler(int signum);


#endif //ASSIGNMENT_3_SCHEDULER_H
