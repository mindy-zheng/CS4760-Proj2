#include <stdio.h>
#include <sys/types.h> 
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h> 
#include <time.h> 

// Process Control Block struct 
struct PCB {
	int occupied;		// either true or false 
	pid_t pid; 		// process id of the child 
	int startSeconds;       // time when it was forked
	int startNano;		// time when it was forked
};

// Help 


int main(int argc, char **argv) {
	// Creating key shared b/w child and parent 
	key_t SH_KEY = ftok(".", 'x'); 
	// Allocating memory associated with key 
	int shm_id = shmget(SH_KEY, sizeof(int) * 10, 0777 | IPC_CREAT);
	if (shm_id <= 0) {
		fprintf(stderr, "Shared memory grab failed \n"); 
		exit(1); 
	}
	
	// Associating pointer to shm_id memory 
	int *shm_ptr = shmat(shm_id, 0, 0); 
	if (shm_ptr == (void *) -1) {
		fprintf(stderr, "Shared memory attach failed \n"); 
		exit(1); 
	}

	// Writing seconds and nanoseconds into the shared memory location 
	struct timespec res; 
	clock_gettime(CLOCK_REALTIME, &res); 
	shm_ptr[0] = res.tv_sec; // Writing the seconds from the time.h struct timespec res
	shm_ptr[1] = res.tv_nsec; // Writing in nanoseconds 	




	return(0); 
}


