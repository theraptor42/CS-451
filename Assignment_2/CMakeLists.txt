cmake_minimum_required(VERSION 2.8)
project(Assignment_2 C)

#set(CMAKE_C_STANDARD 11)

find_package(Threads)
add_executable(Assignment_2 Elevator.c Elevator.h Passenger.h Passenger.c ParseOptions.h ParseOptions.c main.h main.c)
TARGET_LINK_LIBRARIES(Assignment_2 ${CMAKE_THREAD_LIBS_INIT})

#Passenger.h Passenger.c ParseOptions.h ParseOptions.c main.h main.c