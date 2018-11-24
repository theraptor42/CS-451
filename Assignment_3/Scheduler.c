//
// Created by caspi on 11/15/2018.
//

#include "Scheduler.h"


int schedulerProgram()
{
    //TODONE: Parse Schedules
    //Parses the schedule and initializes the processes
    //the processes then wait to be scheduled to execute
    parseSchedule();
    scheduler.runningProcess = -1;

    //TODO: set the timer action (aka the scheduler)
    struct sigaction alarmAction;
    memset(&alarmAction, 0, sizeof (alarmAction));
    alarmAction.sa_handler = timerHandler;
    /* Install the timerHandler as the signal handler for SIGAlRM. */
    sigaction (SIGALRM, &alarmAction, NULL);

    //TODONE: Start the timer
    sleep(2);
    initTimer();
    //TODONE: Loop through the schedule until all the bursts are empty
    int counter;
    int schedulerComplete = 0;

    //The scheduler is triggered by the sigalarm signal
    while(!schedulerComplete)
    {
        schedulerComplete = 1;
        for (counter = 0; counter < scheduler.numberOfProcesses; counter++)
        {
            //If a process still has time remaining, scheduler is not complete
            if (scheduler.processArray[counter].burstTime != 0)
                schedulerComplete = 0;
        }
    }


    return 0;
}

int parseSchedule()
{
    char buffer[STDIN_BUFFER_SIZE];
    char* result;
    int processCount = 0;
    //build the passengers array
    while((result = fgets(buffer, STDIN_BUFFER_SIZE , stdin)) != NULL && result != '\0')
    {
        //Parse the input file to get the process scheduling data
        PROCESS newProcess = constructProcess(buffer);
        //initialize the process to get the PID
        newProcess.pid = initializeProcess(&newProcess);
        //add this process to the array of child processes
        scheduler.processArray[newProcess.processNumber] = newProcess;
        processCount++;
    }

    scheduler.numberOfProcesses = processCount;
    if (processCount == 0)
    {
        fprintf( stdout, "Error: No processes were read from input\n");
        exit(OPTIONS_ERROR_EXIT_STATUS);
    }
    return 0;
}



PROCESS constructProcess(char* inputLine)
{
    //Create a new Process
    PROCESS newProcess;
    //File is assumed to not have errors
    //I also assume, based on the pdf that the values are separated by a space
    sscanf(inputLine, "%d %d %d %d", &newProcess.processNumber, &newProcess.arrivalTime,
            &newProcess.burstTime, &newProcess.priority);

    return newProcess;
}

__pid_t initializeProcess(PROCESS *process)
{
    __pid_t childPid = fork();

    if (childPid == 0)
    {
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
    //pid of the new process
    return childPid;
}

void initTimer()
{
    //Install timer_handler as the signal handler for SIGALRM.
    //The timer goes off 1 second after installation of the timer.*/
    scheduler.timer.it_value.tv_sec = 1;
    scheduler.timer.it_value.tv_usec =0;

    /* ... and every 1 second after that. */
    scheduler.timer.it_interval.tv_sec = 1;
    scheduler.timer.it_interval.tv_usec =0;
    /* Start a real timer. It counts down whenever this process is
    executing. */
    setitimer (ITIMER_REAL, &scheduler.timer, NULL);

    scheduler.currentTime = 0;
}

void timerHandler(int signum)
{
    scheduler.currentTime++;

    int changeCurrentProcess = 0;
    int newProcess = -1;
    int counter = 0;

    if (scheduler.runningProcess != -1 && scheduler.processArray[scheduler.runningProcess].burstTime > 0)
    {
        scheduler.processArray[scheduler.runningProcess].burstTime--;
        if ( scheduler.processArray[scheduler.runningProcess].burstTime == 0)
        {
            fprintf( stdout, "Scheduler: Time Now: %d seconds\n", scheduler.currentTime);
            fprintf( stdout, "Terminating Process %d (Pid %d)\n",
                    scheduler.runningProcess, scheduler.processArray[scheduler.runningProcess].pid);
            kill(scheduler.processArray[scheduler.runningProcess].pid, SIGTERM);
        }
    }


    for (counter = 0; counter < scheduler.numberOfProcesses; counter++)
    {
        PROCESS currentProcess = scheduler.processArray[counter];
        if (newProcess == -1 && currentProcess.burstTime > 0)
        {
            if (currentProcess.burstTime > 0)
                newProcess = currentProcess.processNumber;
        }
        else if (currentProcess.priority < scheduler.processArray[newProcess].priority
                    && currentProcess.burstTime> 0)
        {
            newProcess = currentProcess.processNumber;
        }
    }

    if (newProcess == -1)
    {
        //all processes are complete
    }
    else if (newProcess == scheduler.runningProcess)
    {
        //still the same process running
    }
    else
    {
        //switching to new process

        //suspend running process
        if (scheduler.runningProcess != -1)
        {
            fprintf( stdout, "Scheduler: Time Now: %d seconds\n", scheduler.currentTime);
            fprintf( stdout, "Suspending Process %d (Pid %d)\n",
                     scheduler.runningProcess, scheduler.processArray[scheduler.runningProcess].pid);
            kill(scheduler.processArray[scheduler.runningProcess].pid, SIGTSTP);
        }

        //start new process
        fprintf( stdout, "Scheduler: Time Now: %d seconds\n", scheduler.currentTime);
        fprintf( stdout, "Scheduling Process %d (Pid %d)\n",
                 newProcess, scheduler.processArray[newProcess].pid);
        kill(scheduler.processArray[newProcess].pid, SIGCONT);

        scheduler.runningProcess = newProcess;
    }
}


