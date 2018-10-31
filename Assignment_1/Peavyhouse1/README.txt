Caspian Peavyhouse
Assignment 1

To compile the code:

Navigate to the code folder and 
>> gcc -o 5ps OptionsProcessing.c OptionsProcessing.h Parser.c Parser.h 5ps.c

To run the program,
>> ./5ps -options
 -p <pid> - Specify the PID of the task you want info on
 -s - Get the state information
 -t - Get the run time of the process
 -v - Get the size of the program on memory
 -c - Get the command that started the process

The program fulfills all the critera of the assignment