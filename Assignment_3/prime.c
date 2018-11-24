//
// Created by caspi on 11/15/2018.
//

//Includes
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

//Function Prototypes
int prime(void);
void parseParameters(int argc, char **argv);
void initProcess();
void signalHandler(int signum);

//Global Variables
long unsigned int largestPrime;
int processNumber;
int priority;
__pid_t processPid;
int processStarted = 0;

//defines
#define OPTIONS_ERROR_EXIT_STATUS       2




int main(int argc, char **argv)
{
    //simple container function to organize code
    parseParameters(argc, argv);
    initProcess();

    //start with a 10 digit number
    //I made it odd so I can ignore even numbers
    long unsigned int checkCurrrent = 1234567890; //
    int counter; //counter to count from 3 to n-1 (by 2's)

    //pause the process on creation until it is scheduled
    kill(processPid, SIGTSTP);

    //This will print after the pause, so the first time the process is scheduled
    fprintf(stdout, "Process %d: My priority is %d, my PID is %d: I just got started.\n ",
            processNumber, priority, processPid);
    fprintf(stdout, "I am starting with the number %lu to find the next prime number.\n", checkCurrrent);


    //simple prime checker
    //check each number from
    while (1)
    {
        int isPrime = 1; //is this number prime?
        for (counter = 2; counter < checkCurrrent; counter += 1)
        {
            if (checkCurrrent % counter == 0)
            {
                //the number is not prime
                isPrime = 0;
                break;
            }
        }

        if (isPrime)
        {
            //The number is prime
            largestPrime = checkCurrrent;
         }
        //go to the next odd integer
        checkCurrrent += 1;
    }
}

void signalHandler(int signum)
{
    //Suspend signal
    if (signum == SIGTSTP)
    {
        if (processStarted)
        {
            fprintf(stdout, "Process %d: My priority is %d, my PID is %d: I am about to be suspended.\n",
                    processNumber, priority, processPid);
            fprintf(stdout, "Highest prime number I found is %lu.\n", largestPrime);
        }
    }

    //Continue signal
    else if (signum == SIGCONT)
    {
        if (processStarted)
        {
            fprintf(stdout, "Process %d: My priority is %d, my PID is %d: I just got resumed.\n",
                    processNumber, priority, processPid);
            fprintf(stdout, "Highest prime number I found is %lu.\n", largestPrime);
        }
        processStarted = 1;
    }

    //Terminate signal
    else if (signum == SIGTERM)
    {
        fprintf(stdout, "Process %d: My priority is %d, my PID is %d: I completed my task and I am exiting.\n",
                processNumber, priority, processPid);
        fprintf(stdout, "Highest prime number I found is %lu.\n", largestPrime);
        //the sigterm signal wasn't killing the prime process by itself in testing
        kill(processPid, SIGKILL); //make sure it is dead
    }
}

void initProcess()
{
    //set my default largest prime here in case the process gets killed early
    //so I don't have a null pointer
    largestPrime = 3;


    //Create new sigaction struct and set all its values to 0
    struct sigaction stopAction, alarmAction, killAction;
    memset(&stopAction, 0, sizeof (stopAction));
    memset(&alarmAction, 0, sizeof (alarmAction));
    memset(&killAction, 0, sizeof (killAction));
    //Set my handler to the signal handler
    stopAction.sa_handler = signalHandler;
    alarmAction.sa_handler = signalHandler;
    killAction.sa_handler = signalHandler;

    /* Install the signalHandler as the signal handler for SIGTSTP. */
    sigaction (SIGTSTP, &stopAction, NULL);
    /* Install the signalHandler as the signal handler for SIGALRM. */
    sigaction (SIGCONT, &alarmAction, NULL);
    /* Install the signalHandler as the signal handler for SIGTERM. */
    sigaction (SIGTERM, &killAction, NULL);


    fprintf(stdout, "Process %d has been created and is awaiting scheduling\n", processPid);


}

void parseParameters(int argc, char **argv)
{
    int getOptStatus;
    //The default values will cause an error
    //This ensures that the value is set at creation
    processNumber = -1;
    priority  = -1;
    processPid = -1;

    while ((getOptStatus = getopt (argc, argv, "n:p:d:")) != -1)
    {
        switch (getOptStatus)
        {
            //Sscanf - quick and dirty str -> int conversion
            case 'n':
                sscanf(optarg, "%d", &processNumber);
                break;
            case 'p':
                sscanf(optarg, "%d", &priority);
                break;
            case 'd':
                sscanf(optarg, "%d", &processPid);
                break;

                //This is what it does when it doesn't recognize a character
            case '?':
                if (optopt == 'n' || optopt == 'p' || optopt == 'd')
                {
                    //The flag was set but no argument was provided
                    printf("There was an error parsing the internal -%c option.\n", optopt);
                    exit(OPTIONS_ERROR_EXIT_STATUS);
                }
                else
                {
                    printf("Unknown option `-%c'.\n", optopt);
                    exit(OPTIONS_ERROR_EXIT_STATUS);
                }

            default:
                exit(OPTIONS_ERROR_EXIT_STATUS);
        }
    }


    if (processNumber == -1 || processPid == -1 || priority == -1)
    {
        printf("There was an error parsing the internal options.  One or more arguments were not set\n");
        exit(OPTIONS_ERROR_EXIT_STATUS);
    }
}