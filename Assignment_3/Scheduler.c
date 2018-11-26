//
// Created by caspi on 11/15/2018.
//

#include "Scheduler.h"


int schedulerProgram()
{
    //TODONE: Parse Schedules

    //Parses the schedule and creates the process structs
    //the processes then wait to be scheduled to execute
    parseSchedule();

    //TODONE: set the timer action (aka the scheduler)
    struct sigaction alarmAction;
    memset(&alarmAction, 0, sizeof (alarmAction));
    alarmAction.sa_handler = timerHandler;
    /* Install the timerHandler as the signal handler for SIGAlRM. */
    sigaction (SIGALRM, &alarmAction, NULL);
    //TODONE: Start the timer
    sleep(2);
    initTimer();

    //The scheduler is triggered by the sigalarm signal
    //Wait for all processes to complete
    while(scheduler.numberOfProcesses > scheduler.processesCompleted);



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
        PROCESS newProcess;
        constructProcess(buffer, &newProcess);

        //add this process to the array of child processes
        scheduler.processArray[newProcess.processNumber] = newProcess;
        processCount++;
    }

    scheduler.numberOfProcesses = processCount;
    scheduler.processesCompleted = 0;

    if (processCount == 0)
    {
        fprintf( stdout, "Error: No processes were read from input\n");
        exit(OPTIONS_ERROR_EXIT_STATUS);
    }
    return 0;
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
    //Increment the current time
    scheduler.currentTime++;
    fprintf(stdout, "Timer: %d\n", scheduler.currentTime);
    ProcessNode *oldHead = scheduler.priorityQueue.head;
    int headUnchanged = 1;


    //Check if the head has completed
    if (scheduler.priorityQueue.head != NULL)
    {
        //if my head process burst time has reached 0
        if (scheduler.priorityQueue.head->process->burstTime <= 0)
        {
            //The process has executed all of its burst time
            ProcessNode *completedNode = removeHeadFromPLinkedList(&scheduler.priorityQueue);
            //Terminate it
            terminateProcess(completedNode->process, scheduler.currentTime);
            scheduler.processesCompleted++;
            headUnchanged = 0;
        }
    }

    //Check if any processes have arrived at this time
    int counter;
    for (counter = 0; counter < scheduler.numberOfProcesses; counter++)
    {
        if (scheduler.processArray[counter].arrivalTime == scheduler.currentTime)
        {
            //This process has just arrived
            fprintf(stdout, "Process Arrived: %d at arrival time%d\n", counter, scheduler.processArray[counter].arrivalTime);
            //add it to the queue
            ProcessNode newNode;
            newNode.process = &scheduler.processArray[counter];
            //will be zero if this was added to the front of the linked list
            if (addPNodeToPLinkedList(&scheduler.priorityQueue, &newNode) == 0)
            {
                headUnchanged = 0;
            }
        }
    }


    //Decrement the current process burst time
    if (scheduler.priorityQueue.head != NULL)
    {
        //if the head changes, I need to suspend the running process and resume the head
        if (!headUnchanged)
        {
            if (oldHead != NULL)
                suspendProcess(oldHead->process, scheduler.currentTime);
            if(scheduler.priorityQueue.head != NULL)
                resumeProcess(scheduler.priorityQueue.head->process, scheduler.currentTime);

        }

        //Decrement the head's burst time
        burstProcessOneSecond(scheduler.priorityQueue.head->process);
    }

}


