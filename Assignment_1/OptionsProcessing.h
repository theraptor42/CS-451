/*
 *Author: Caspian Peavyhouse
 *Assignment Number: A1
 *Date of Submission: 10/17/2018
 *Name of this file: OptionsProcessing.h
 *Description of the program:
 * OptionsProcessing - a module for processing
 *          the command line arguments for 5ps
 */
#ifndef ASSIGNMENT_1_5PS_H
#define ASSIGNMENT_1_5PS_H

//Define the option mask values
#define C_FLAG              0x0001
#define S_FLAG              0x0010
#define T_FLAG              0x0100
#define V_FLAG              0x1000

//define some generic stuff
#define ERROR_EXIT_STATUS   (-1)
#define DEFAULT_PID         "1"

//define some magic numbers
//PID max value is 32768 (cat /proc/sys/kernel/pid_max)
// So 6 = 5 digits + null terminator
#define PID_MAX_LENGTH      6
#define PROC_PATH           "/proc/"
#define PROC_PATH_LENGTH    6

extern unsigned short readOptions(int argc, char **argv, char *taskPidPath, char *taskPid);
int generateDirPath(char *taskPID, char* dirPath);
int processDirExists(char *dirPath);

#endif //ASSIGNMENT_1_5PS_H
