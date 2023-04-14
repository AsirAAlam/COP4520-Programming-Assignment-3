Name: Asir Alam
COP4520 Programming Assignment 3
Problem 2: Atmospheric Temperature Reading Module

Instructions:
- Compile: g++ -O2 problem2.cpp -pthread
- Execute: ./a.out

I am reusing my implementation of the concurrent linked list from problem 1 as
the shared data storage for this problem. I modified it slightly to fit the 
purposes of this problem. It now accepts a "Reading" class intead of integers. 
Insertion now happens at the head instead of in sorted order, since it is no 
longer required. This is now effectively a concurrent stack. Each thread 
independently generates a random reading and adds it to the data storage over
60 timesteps, to simulate the 60 minutes of an hour. At the end of the hour the
overall data is used to extract the desired information as per the Assignment
instructions. For simulating multiple hours, the simulation can be run multiple
times. The concurrent linked list ensures mutual exclusion, so progress is 
guaranteed.

I thoroughly analyzed the program using debug statements to verify the behavior
of the program, which is correct as per the instructions. The time complexity 
of this program is O(nlogn) due to the sorting operations, where n is the number 
of time steps per sensor. However, since the input is restricted to 60 time steps
the run-time is rather insignificant. 