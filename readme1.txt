Name: Asir Alam
COP4520 Programming Assignment 3
Problem 1: The Birthday Presents Party

Instructions:
- Compile: g++ -O2 problem1.cpp -pthread
- Execute: ./a.out

For my concurrent linked list implementation, I used lazy synchronization. 
This implementation adds a lock and a boolean for locking and marking the nodes,
respectively. Adding involves locking the target location's predecessor and
inserting. Deletion involves two steps. Marking the target node, which logically
removes it, and redirecting its predecessor's link, which physically removes it.
I am only deleting from the front of the list since rules for removing from the 
chain were not specified. Checking whether a value exists involves iterating over 
the list, which is wait-free. For adding operations, I made sure to add in sorted 
order by iterating to the correct position. 

The simulation was implemented as follows. I generated a random permutation of 
the first 500,000 positive integers to simulate the random ordering of picking 
gifts from the large bag. An atomic indexing variable is used by the threads to
index the next gift to choose from the bag. Each thread alternates between 
removing a gift from the bag and adding it to the chain, and removing it from the
chain and writing a card. If all the gifts have been removed from the bag, they
will only write cards. 

I verified the behavior of the program by running it on a smaller number of gifts
and using helpful debug statements. It is correct as per the instructions. 
The runtime is linear with respect to the number of presents since at any given 
moment there can be at most 4 presents in the chain due to the alternating behavior 
and since we are accessing each gift a constant number of times. I verified this by 
running the programming on increasing numbers of cards and observing the run-time. 
