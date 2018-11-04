Caspian Peavyhouse
README.txt
Assignment 2

To compile the code:

Navigate to the code folder and 
>> gcc -o outfile Elevator.h Elevator.c Passenger.h Passenger.c ParseOptions.h ParseOptions.c main.h main.c -lpthread

To run the program,
>> ./outfile -{options}* < {inputFilePath}
Example >> ./outfile -p 1 -f 6 -w 8 -d  < ./input.txt

 -f <number of floors> - Specify number of floors in the building
 -p <number of people> - Specify number of people in the building
 -w <max wander time> - Specify the max wander time of the people
 -d - (Extra Credit) Passengers wait for the elevator to move the right direction.
 
 if some of the parameters don't match what is provided (wrong number of schedules, too few floors)
 the program will tell you where the error is
 
 inputFilePath - path to the file with the passengers schedules
				** if this is not provided, the program will hang
				The format of the input file is the same as what is provided in the assignment document


				 
 The program fulfills all the critera of the assignment
 Extra Credit - the program meets the extra credit as well
			If you wish to run in that mode, use the -d flag
			
**I have used a coupon for this assignment