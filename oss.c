#include <stdio.h>
#include <sys/types.h> 
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>  
#include <time.h> 
#include <stdbool.h>

// Process Control Block struct 
typedef struct PCB {
	int occupied;		// either true or false 
	pid_t pid; 		// process id of the child 
	int startSeconds;       // time when it was forked
	int startNano;		// time when it was forked
} PCB;

// Help menu 
void help() {
        printf("oss [-h] [-n proc] [-s simul] [-t timelimit]");
        printf("\n\t Help Menu \n");
        printf("     -----------------\n");
	printf(" -h option: displays help message\n"); 
	printf(" -n [proc]: number of total children to launch\n"); 
	printf(" -s [simul]: indicates how many children can run simultaneously\n"); 
	printf(" -t [timelimit]: will call a time within the bounds of 1 - specified time limit \n"); 
}

PCB processTable[20]; 


// Variables for incrementing clock 
#define NANO_INC 10000000   // represents 10 milliseconds 
#define NANO_SEC 1000000000 // 1 second equals a billion nanosec 
#define HALF_SEC 500000000  // Every half second 

int main(int argc, char **argv) {

	// Generate shared key shared b/w child and parent 
	key_t SH_KEY = ftok(".", 'x'); 
	
	// Allocating memory associated with key 
	int shm_id = shmget(SH_KEY, sizeof(int) * 2, 0777 | IPC_CREAT);
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

	// Variables to hold 
	int sys_sec = 0; 
	int sys_nano = 0; 
	
	int opt; // integer input from command line 	
	int n = 0; // number of worker processes to be updated 
	int s = 0; 
	int t = 0;
	int simul; 
	int timelimit; 
	// Command line arguments and user input 
	while ((opt = getopt(argc, argv, "hn:s:t:")) != -1) {
			switch(opt) {
			case 'h': 
				help(); 
				return EXIT_SUCCESS;
			case 'n':
				n = atoi(optarg);
				break; 
			case 's': 
				simul = atoi(optarg); 
				break;
			case 't': 
				timelimit = atoi(optarg); 
				break; 
			default:
				help(); 
				return EXIT_FAILURE; 	
				}
		}



	bool stillChildrenToLaunch = true;
	
	
	// Launchin worker processes and create child processes: 
	for (int i = 0; i < n+1; i++) {
    		pid_t childPid = fork();
    		if (childPid == 0) { // C
        	// 0 indiciates the child process
		// clock must be accessible to children 
	 		char sec_str[10], nano_str[10]; 
        		sprintf(sec_str, "%d", sys_sec);
        		sprintf(nano_str, "%d", sys_nano);

			execl("./worker", "./worker", sec_str, nano_str, NULL);
        		perror("Execl() failed");
        		exit(0);  // Only reached if exec() fails.
    		} else if (childPid > 0) {
        	// Incidates parent process
			for (int j = 0; j < 20; j++) {
            			if (!processTable[j].occupied) {
                			processTable[j].occupied = 1;
                			processTable[j].pid = childPid + i;
                			processTable[j].startSeconds = sys_sec;
                			processTable[j].startNano = sys_nano;
                			break;
            			}
        		}
		} else {
        	// Fork failed.
        		fprintf(stderr, "Fork failed.\n");
        		exit(1);
   			}
		}

	// Wait for child processes to terminate
	for (int i = 0; i < n+1; i++) {
    		wait(NULL);
	}	
	
	// Iterations 
	int iteration = 0; 
	// Writing seconds and nanoseconds into the shared memory location and updating it 
	while (stillChildrenToLaunch == true && simul > 0) {
    		// Updating system clock 
		sys_nano += NANO_INC;
  		 if (sys_nano >= NANO_SEC) {
        		sys_sec++;
        		sys_nano -= NANO_SEC;
    		}

    		shm_ptr[0] = sys_sec; // Writing in seconds
    		shm_ptr[1] = sys_nano; // Writing in nanoseconds        


		if (sys_nano % HALF_SEC == 0) { // Every half a second of simulated clock time
			printf("Process Table:\n");
     		       	printf("Entry \t Occupied \t PID \t StartSec\t StartNano \n"); 
        		for (int i = 0; i < 20; i++) {
			printf(" %d \t %d \t\t %d \t\t%d \t\t\t %d \n", i, processTable[i].occupied, processTable[i].pid, processTable[i].startSeconds, processTable[i].startNano);
        		}
    		}
		iteration++; 
		if (iteration % (HALF_SEC / NANO_INC) == 0) {
       			 // Sleep for half a second of real time, or else infinite loop
        		usleep(HALF_SEC);
    		}


   		int status;
    		pid_t pid = waitpid(-1, &status, WNOHANG); // Non-blocking wait() call
    		if (pid > 0) { // Child has terminated
        		for (int i = 0; i < 20; i++) {
            			if (processTable[i].pid == pid) {
                		processTable[i].occupied = 0;
                		break;
            			}
        		}
			if (--simul == 0) { 
				stillChildrenToLaunch = false; 
			} 
        		if (--simul > 0) { // Launch new child process until simul reaches -s 
				// Generate random limit between 1 and -t 
				int random_sec = rand() % timelimit + 1; 
				int random_nano = random_sec * NANO_SEC; // Converts random generated num for random_sec into nanoSeconds
				char sec_str[10], nano_str[10]; 
				sprintf(sec_str, "%d", sys_sec + random_sec); 
				sprintf(nano_str, "%d", sys_nano + random_nano); 
				
           
				pid_t new_pid = fork();
            			if (new_pid == 0) {
                		// This is the child process. Replace it with a new program using exec().
					execl("./worker", "./worker", sec_str, nano_str, NULL); 
					perror("Execl() failed"); 
                			exit(0);  // Only reached if exec() fails.
            			} else if (new_pid > 0) {
                		// This is the parent process. Update the process table.
                		for (int i = 0; i < 20; i++) {
                    			if (processTable[i].occupied == 0) {
                        			processTable[i].occupied = 1;
                        			processTable[i].pid = new_pid;
                        			processTable[i].startSeconds = sys_sec;
                        			processTable[i].startNano = sys_nano;
                        			break;
                    			}
                		}
            		} else {
                	// Fork failed 
                		fprintf(stderr, "Fork failed.\n");
                		exit(1);
            			}
			}
	
		}
        	
	}

	

	// Detatch from shared memory 
	shmdt(shm_ptr);  
	



	return(0); 
}
