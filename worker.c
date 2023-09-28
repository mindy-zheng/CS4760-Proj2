#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/wait.h>
#include <time.h> 
#include <sys/ipc.h>
#include <sys/shm.h>
 
int main(int argc, char **argv) {
	
	int sec  = atoi(argv[1]);		// Collect user input - seconds 
	int nano = atoi(argv[2]); 	// Collects user input - nanoseconds 	
	key_t SH_KEY = ftok(".", 'x');


	if (argc < 3) { 
		fprintf(stderr, "Usage: %s seconds nanoseconds\n", argv[0]); 
		exit(1); 
	}
	
	// Getting shared memory ID 
	int shm_id = shmget(SH_KEY, sizeof(int) * 10, 0777 | IPC_CREAT); 
	if (shm_id <= 0) {
		fprintf(stderr, "Shared memory grab failed \n");
		exit(1); 
	}

	// Attaching to shared memory from oss.c 
	int *shm_ptr = shmat(shm_id, 0, 0); 
	if (shm_ptr == (void *) -1) {
		fprintf(stderr, "Shared memory attach failed \n"); 
		exit(1); 
	} 

	// Read clock on the shared memory 
	int sys_sec = shm_ptr[0]; 
	int sys_nano = shm_ptr[1];  	
	
	// Calculate target time to terminate system 
	int terminate_sec = sec + sys_sec; 
	int terminate_nano = nano + sys_nano; 
	
	// Output results
	printf("WORKER PID: %d PPID: %d SysClockS: %d SysClockNano: %d TermTimeS: %d TermTimeNano: %d\n -- Just Starting", getpid(), getppid(), sys_sec, sys_nano, terminate_sec, terminate_nano); 


	return 0; 
}
