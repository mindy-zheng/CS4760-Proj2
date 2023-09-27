# CS4760-Proj2
This project builds on concepts from Proj1. To gain a better understanding of this project myself, I'll write a brief overview of what is to be expected. 

The final executables of this project will be: oss and worker. 

The job of the oss will be to launch workers while maintaining a simulated system clock in shared memory. *This sys clock is not tied to the main clock

The clock consists of 2 separate integers: seconds and nanoseconds, in shared memory. (Initialize these to zero) 

*System clock must be accessible by the children, so it must be in shared memory* 

Children processes will not be modifying this clock (only need to view) 

oss will also maintain a process table (consisting of PCB, one for each process and will not be in shared memory). 
- will keep track of the PID of the child process and the time before oss does a fork to launch a child process. 

- should also contain an entry if entry in the Process table is empty or not being used. 


