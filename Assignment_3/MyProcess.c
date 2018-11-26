//
// Created by caspi on 11/25/2018.
//

#include "MyProcess.h"

void constructProcess(char* inputLine, PROCESS *newProcess)
{
    //Construct a process from an input line

    //File is assumed to not have errors
    //I also assume, based on the pdf that the values are separated by a space
    sscanf(inputLine, "%d %d %d %d", &newProcess->processNumber, &newProcess->arrivalTime,
           &newProcess->burstTime, &newProcess->priority);
    //default value till initialization
    newProcess->pid = -1;
}

__pid_t initializeProcess(PROCESS *process, int currentTime)
{
    if (process == NULL)
        return -2;

    __pid_t childPid = fork();
    process->pid = childPid;
    if (childPid == 0)
    {
        //Only the new child process enters this block
        //put all the arguments into char arrays
        char processNumberArg[10];
        char processPriorityArg[10];
        char processPIDArg[10];
        sprintf(processNumberArg, "-n %d", process->processNumber);
        sprintf(processPriorityArg, "-p %d", process->priority);
        sprintf(processPIDArg, "-d %d", getpid());

        //child process switch task
        execlp("./prime.o", "main", processNumberArg, processPriorityArg, processPIDArg, NULL);
    }

    fprintf( stdout, "Scheduler: Time Now: %d seconds\n", currentTime);
    fprintf( stdout, "Arrival of process %d (Pid %d)\n", process->processNumber, childPid);


    fprintf( stdout, "Child pid: %d\n", childPid);
    fprintf( stdout, "Process pid: %d\n", process->pid);

    //parent returns the pid of the new process
    return childPid;
}


int resumeProcess(PROCESS *process, int currentTime)
{
    if (process == NULL)
        return 1;

    if (process->pid == -1)
    {
        int processPid = initializeProcess(process, currentTime);
        process->pid = processPid;
    }
    else
    {
        fprintf( stdout, "Scheduler: Time Now: %d seconds\n", currentTime);
        fprintf( stdout, "Resuming Process %d (Pid %d)\n", process->processNumber, process->pid);
        //send the resume signal
        kill(process->pid, SIGCONT);
    }

    return 0;
}


int suspendProcess(PROCESS *process, int currentTime)
{
    if (process == NULL || process->pid == -1)
        return 1;
    fprintf(stdout, "Current time  %d\n", currentTime);
    fprintf( stdout, "Scheduler: Time Now: %d seconds\n", currentTime);
    fprintf( stdout, "Suspending Process %d (Pid %d)\n", process->processNumber, process->pid);
    //Send the suspend signal
    kill(process->pid, SIGTSTP);

    return 0;
}


int terminateProcess(PROCESS *process, int currentTime)
{
    if (process == NULL || process->pid == -1)
        return 1;

    fprintf( stdout, "Scheduler: Time Now: %d seconds\n", currentTime);
    fprintf( stdout, "Terminating Process %d (Pid %d)\n", process->processNumber, process->pid);
    //Send the termination signal
    kill(process->pid, SIGTERM);

    return 0;
}

int burstProcessOneSecond(PROCESS* process)
{
    if (process == NULL)
        return 1;

    //Run the process for one second and return the remaining burst time
    process->burstTime--;
    return process->burstTime;
}