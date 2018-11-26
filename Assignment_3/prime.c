//
// Created by caspi on 11/15/2018.
//

//Includes
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <math.h>

//Function Prototypes
void parseParameters(int argc, char **argv);
void initProcess();
void continueSignalHandler(int signum);
void suspendSignalHandler(int signum);
void terminateSignalHandler(int signum);

//Global Variables
volatile long unsigned int largestPrime;
int processNumber;
int priority;
__pid_t processPid;

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

    //This will print the first time the process is scheduled
    fprintf(stdout, "Process %d: My priority is %d, my PID is %d: I just got started.\n ",
            processNumber, priority, processPid);
    fprintf(stdout, "I am starting with the number %lu to find the next prime number.\n", checkCurrrent);


    //simple prime checker
    //check each number from
    while (1)
    {
        int isPrime = 1; //is this number prime?
        long double primeCheckLimit = sqrtl((long double)checkCurrrent);

        for (counter = 2; counter < primeCheckLimit; counter += 1)
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

void continueSignalHandler(int signum)
{
    //resume signal
    fprintf(stdout, "Process %d: My priority is %d, my PID is %d: I just got resumed.\n",
            processNumber, priority, processPid);
    fprintf(stdout, "Highest prime number I found is %lu.\n", largestPrime);
    fflush(stdout);
}

void suspendSignalHandler(int signum)
{
    //Suspend signal
    fprintf(stdout, "Process %d: My priority is %d, my PID is %d: I am about to be suspended.\n",
            processNumber, priority, processPid);
    fprintf(stdout, "Highest prime number I found is %lu.\n", largestPrime);
    fflush(stdout);
}

void terminateSignalHandler(int signum)
{
    //Terminate signal
    fprintf(stdout, "Process %d: My priority is %d, my PID is %d: I completed my task and I am exiting.\n",
            processNumber, priority, processPid);
    fprintf(stdout, "Highest prime number I found is %lu.\n", largestPrime);
    //the sigterm signal wasn't killing the prime process by itself in testing
    fflush(stdout);
    kill(processPid, SIGKILL); //make sure it is dead
}

void initProcess()
{
    //set my default largest prime here in case the process gets killed early
    //so I don't have a null pointer
    largestPrime = 3;


    //Create new sigaction struct and set all its values to 0
    struct sigaction suspendAction, continueAction, terminateAction;
    memset(&suspendAction, 0, sizeof (suspendAction));
    memset(&continueAction, 0, sizeof (continueAction));
    memset(&terminateAction, 0, sizeof (terminateAction));
    //Set my handler to the signal handler
    suspendAction.sa_handler = suspendSignalHandler;
    continueAction.sa_handler = continueSignalHandler;
    terminateAction.sa_handler = terminateSignalHandler;

    /* Install the signalHandler as the signal handler for SIGTSTP. */
    sigaction (SIGTSTP, &suspendAction, NULL);
    /* Install the signalHandler as the signal handler for SIGALRM. */
    sigaction (SIGCONT, &continueAction, NULL);
    /* Install the signalHandler as the signal handler for SIGTERM. */
    sigaction (SIGTERM, &terminateAction, NULL);


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