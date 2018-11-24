cmake_minimum_required(VERSION 3.10)
project(Assignment_3 C)

set(CMAKE_C_STANDARD 99)

add_executable(prime.o prime.c)
add_executable(assign_sched.o main.c Scheduler.c Scheduler.h)