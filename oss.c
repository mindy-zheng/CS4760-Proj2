#include <stdio.h>
#include <sys/types.h> 
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/wait.h>
#include <sys/ipc.h>

// Process Control Block struct 
struct PCB {
	int occupied;		// either true or false 
	pid_t pid; 		// process id of the child 
	int startSeconds;       // time when it was forked
	int startNano;		// time when it was forked
}

// Help menu 

void help() {
        printf("oss [-h] [-n proc] [-s simul] [-t iter]");
        printf("\n\t Help Menu \n");
        printf("     -----------------\n");
	printf(" -h option: displays help message\n"); 
	printf(" -n [proc]: number of total children to launch\n"); 
	printf(" -s [simul]: indicates how many children can run simultaneously\n"); 
	printf(" -t [timelimit]: bound of time that a child process will be launched for"); 
}

// Parent and child share common key 
const int sh_key = 1000

int main(int argc, char **argv) {
	// Allocating memory associated with key 
	int shm_id = shmget(sh_key, sizeof(int) * 10, 0777 | IPC_CREAT);
	if (shm_id <= 0) {
		fprintf(stderr, "Shared memory grab failed \n"); 
		exit(1); 
	}
	
	// Associating pointer to shm_id memory 
	int *shm_ptr = shmat(shm_id, 0, 0); 
	if (shm_ptr <= 0) {
		fprintf(stderr, "Shared memory attach failed \n"); 
		exit(1); 
	}

	




	return 0; 
}


