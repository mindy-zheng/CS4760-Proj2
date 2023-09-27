#include <stdio.h>
#include <sys/types.h> 
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/wait.h>

// Process Control Block struct 
struct PCB {
	int occupied;		// either true or false 
	pid_t pid; 		// process id of the child 
	int startSeconds;       // time when it was forked
	int startNano;		// time when it was forked
}



int main(int argc, char **argv) {






	return 0; 
}


